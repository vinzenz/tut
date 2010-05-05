#include "libunwind.c"
#include <elfutils/libdw.h>

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <argp.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C"
{
void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));
}

int f3()
{
    printf("%s\n", __PRETTY_FUNCTION__);
    return 4321;
}

int f1(int p_a, int p_b, const char* path, int nr, int a)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    return 23;
}

void f2(int i)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    printf("START\n");
    f1(87, 123, "dupa", 321, 24);
    printf("END\n");
}


int f1_new(int p_a, int p_b, const char* path, int nr, int a)
{
    printf("START\n");
    f3();
    return 0;
}
void* ptr = NULL;
void* ptr2 = NULL;
unw_word_t ip;
unw_cursor_t cursor;
unw_context_t uc;
unw_word_t sp;
int* result;
extern char* __progname;


void get_function_dies(Dwarf_Die **scopes, int *nscopes, Dwarf_Addr addr, Dwarf* dbg)
{
	Dwarf_Die result_cu;
	Dwarf_Die *cu_die;

	cu_die = dwarf_addrdie(dbg, addr, &result_cu);
    printf("\naddr=%p\n", addr);
    printf("%x\n", cu_die->cu);
    printf("%x\n", uc);

	if (!cu_die)
    {
	    printf("ununable to get the compiler Unit for the address: %p", addr);
        exit(-1);
    }

	*nscopes = dwarf_getscopes(cu_die, addr, scopes);

	if((*nscopes) < 0)
    {
        printf("ununable to get the function die at address %p", addr);
    }
}

ulong fetch_register(int reg)
{
    unw_word_t value;

	switch(reg)
    {
      case 0:
                //printf("GET_EAX\n");
                unw_get_reg(&cursor, UNW_X86_EAX, &value);
                return value;

      case 1:
                //printf("GET_ECX\n");
                unw_get_reg(&cursor, UNW_X86_ECX, &value);
                return value;

      case 2:
                //printf("GET_EDX\n");
                unw_get_reg(&cursor, UNW_X86_EDX, &value);
                return value;

      case 3:
                //printf("GET_EBX\n");
                unw_get_reg(&cursor, UNW_X86_EBX, &value);
                return value;

      case 4:
                //printf("GET_ESP\n");
                unw_get_reg(&cursor, UNW_X86_ESP, &value);
               return value;

      case 5:
                //printf("GET_EBP\n");
                unw_get_reg(&cursor, UNW_X86_EBP, &value);
                return value;

      case 6:
                //printf("GET_ESI\n");
                unw_get_reg(&cursor, UNW_X86_ESI, &value);
                return value;

      case 7:
                //printf("GET_EDI\n");
                unw_get_reg(&cursor, UNW_X86_EDI, &value);
                return value;
      default:
                printf("unhandled x86 register %d", reg);
                exit(-1);
	}

	return 0;
}

ulong translate(Dwarf_Op *locexpr, size_t len, ulong *value ,bool *need_fb)
{
	int i;
	unsigned int reg;
	unsigned long loc = 0;
	unsigned long offset = 0;
	for (i=0; i<len; i++)
    {
	switch (locexpr[i].atom)
    {
			case DW_OP_reg0 ... DW_OP_reg31:
	  			reg = locexpr[i].atom - DW_OP_reg0;
				goto op_reg;
			case DW_OP_regx:
				reg = locexpr[i].number;
			op_reg:
				*value = fetch_register(reg);
				break;
			case DW_OP_fbreg:
				*need_fb = true;
				loc = locexpr[i].number;
	  			break;
			case DW_OP_addr:
				loc = locexpr[i].number;
				break;
			case DW_OP_breg0 ... DW_OP_breg31:
				reg = locexpr[i].atom - DW_OP_breg0;
				offset = locexpr[i].number;
				loc = fetch_register(reg) + offset;
				break;
			case DW_OP_bregx:
				reg = locexpr[i].number;
				offset = locexpr[i].number2;
				loc = fetch_register(reg) + offset;
				break;
			default:
                printf("unprocessed OpCode in translate()");
                exit(-2);
				break;
		}
	}
	return loc;
}

Dwarf_Op *get_location(Dwarf_Attribute *location, Dwarf_Addr addr, size_t *len)
{
	Dwarf_Op *expr;
	switch(dwarf_getlocation_addr(location, addr, &expr,len,1))
	{
	case 1:
		if( *len != 0)
			break;
	case 0:
	default:
		return NULL;
	}
    return expr;
}

int var_type(Dwarf_Die *typedie, char space)
{
	int tag;
	Dwarf_Attribute attr_mem;
	Dwarf_Die die_mem;
	Dwarf_Die *die;
	const char *name;
	int size = 0;

    if (typedie == NULL)
    {
		printf("%c<no type>", space);
    }
	else
	{
		name = dwarf_diename (typedie);
		if (name != NULL)
        {
			printf ("%c%c%s: ", space, space, name);
        }

        die = dwarf_formref_die( dwarf_attr_integrate(typedie, DW_AT_type, &attr_mem), &die_mem );

		tag = dwarf_tag (typedie);
		name = dwarf_diename (die);

		if (name)
        {
			printf("%c%c%s: ", space, space, dwarf_diename (die));
        }

		switch (tag)
		{
			case DW_TAG_pointer_type:
				size = dwarf_bytesize(die);
				break;
			case DW_TAG_array_type:
				printf ("[]");
				break;
			case DW_TAG_const_type:
				printf(" const");
				break;
			case DW_TAG_volatile_type:
				printf (" volatile");
				size = dwarf_bytesize(die);
				break;
			case DW_TAG_base_type:
				size = 4;
				break;
			default:
				printf("%c<unknown %#x>", space, tag);
				break;
		}
	}

	return size;
}

ulong variable_address(Dwarf_Die *fun_die, Dwarf_Die *var_die, ulong *value, Dwarf_Addr addr)
{
	size_t len;
	Dwarf_Op *locexpr;
	bool need_fb = false;
	Dwarf_Attribute fb_attr, loc_attr;
	ulong var_addr = 0;

    if(!dwarf_attr_integrate(var_die, DW_AT_location, &loc_attr))
    {
		goto out;
    }

	locexpr = get_location(&loc_attr, addr, &len);
	if (!locexpr)
    {
		goto out;
    }

	var_addr = translate(locexpr, len, value, &need_fb);

//    printf("need_fb=%d\n", need_fb);

	if (need_fb)
    {

   		if(!dwarf_attr_integrate(fun_die, DW_AT_frame_base, &fb_attr))
        {
            printf("OUT\n");
			goto out;
        }

		locexpr = get_location(&fb_attr, addr, &len);
		if (!locexpr)
        {
            printf("LOCOUT\n");
			goto out;
        }

		var_addr += translate(locexpr, len, value, 0);
	}
	return var_addr;
out:

    return 0;
}

void print_variable(Dwarf_Die *fun_die, Dwarf_Die *var_die)
{
	size_t size;
	ulong var_addr = 0;
	ulong value = 0;
	int pointer_desize;
	Dwarf_Attribute attr_mem;
    Dwarf_Die typedie_mem;

	printf("\n %s\t", dwarf_diename(var_die));
    Dwarf_Die *typedie = dwarf_formref_die (dwarf_attr_integrate (var_die, DW_AT_type, &attr_mem), &typedie_mem);

	pointer_desize = var_type(typedie, '\t');
	size = dwarf_bytesize(typedie);
	var_addr = variable_address(fun_die,var_die, &value, ip);

    if (!var_addr && !value)
    {
		printf("\t Dwarf information not available");
		return;
	}

    if(!value && var_addr)
    {
        int* va = (int*)var_addr;
        /*printf("VAR_ADDR=%d\n", *va);*/

/*       printf("\n%d\n", size);*/
        /*printf("%d\n", pointer_desize);*/

       /*        if (size != ULONG_MAX)*/
        /*{*/
            /*memcpy(&value, &var_addr, size);*/
        /*}*/
		/*else*/
        /*{*/
            /*memcpy(&value, &var_addr, pointer_desize);*/
        /*}*/

        if (pointer_desize && size != ULONG_MAX)
        {
			char ch;
			char *p = (char *)value;
			short int s;
			int i;
			ulong l;

			switch (pointer_desize)
            {
				case 1: /* char * */
					printf("%s\n", *va);
					break;
				case 2: /* short int * */
//					read_vmcore(value, 2, &s);
					printf("%d\n", *va);
					break;
				case 8: /* long * */
//					read_vmcore(value, 2, &l);
//					printf("%ld\n", l);
					break;
				case 4: /* int * */
//                    memcpy(&i, &value, 4);
//
                    memcpy(&i, &*va, 4);
		    printf("%d\n", i);
					/*printf("%d\n", *va);*/
					break;
			}
		}
        else
        {
//			printf("\tvalue: %ld",value);
        }

	} else
    {
//		printf("\tvalue:%ld",value);
    }
}

void print_variables(Dwarf_Die *fun_die)
{
	Dwarf_Die result;
	if (dwarf_child (fun_die, &result) == 0)
    {
    	do
        {
		 switch (dwarf_tag (&result))
         {
              	case DW_TAG_variable:
//					print_variable(fun_die, &result);
				break;
	               	case DW_TAG_formal_parameter:
					print_variable(fun_die, &result);
				break;
                  	default:
				break;
		}
	   }
       while(dwarf_siblingof(&result,&result) == 0);
    }
}

void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
    if(this_fn == ptr)
    {
        printf("inside\n");

        getcontext(&uc);
        unw_init_local(&cursor, &uc);

        unw_step(&cursor);
        unw_get_reg(&cursor, UNW_REG_IP, &ip);

        if (elf_version(EV_CURRENT) == EV_NONE)
        {
            printf("error\n");
            exit(-1);
        }

        int fd = open(__progname, O_RDONLY);
        if(fd < 0)
        {
             printf("error\n");
             exit(-1);
        }

        Elf* elf_descriptor = elf_begin(fd, ELF_C_READ, (Elf *)0);
        if(!elf_descriptor)
        {
             printf("error\n");
             exit(-1);
        }

        Dwarf* dbg = dwarf_begin_elf(elf_descriptor, DWARF_C_READ, NULL);
        if(!dbg)
        {
             printf("error\n");
             exit(-1);
        }

        getcontext(&uc);
        unw_init_local(&cursor, &uc);

        unw_step(&cursor);
        unw_get_reg(&cursor, UNW_REG_IP, &ip);

        Dwarf_Die *scopes;
        int nscopes;
        int index;
        scopes = NULL;

        get_function_dies(&scopes, &nscopes, (Dwarf_Addr)ip, dbg);
        for(index = 0; index < nscopes; index++)
        {
            switch(dwarf_tag(&scopes[index]))
            {
              case DW_TAG_subprogram:
                  printf("\n%x - %d\n", dwarf_tag(&scopes[index]), &scopes[index]);
                    print_variables(&scopes[index]);
                    break;
                default:
                    break;
            }
        }

        close(fd);
        //no instrument

        //stub function
        ///important it's only type
        f1_new(1,2,"",4,5);
        //here return value can be simple return

        //instrument     unw_getcontext(&uc);
        unw_step(&cursor);
        unw_resume(&cursor);
    }
}

void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
}


int main (int argc, char** argv)
{
    int remaining;

    printf("manual test: %s\n", __progname);
    ptr=(void *)&f1;
    /*ptr2=&f3;*/

   f2(2);
   printf("JEST OK\n");
    /*f3();*/

    return 0;
}

