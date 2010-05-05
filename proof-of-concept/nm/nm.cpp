#include <elfutils/libdw.h>
#include <ctype.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <libintl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cxxabi.h>

#include <iostream>


typedef struct GElf_SymX
{
  GElf_Sym sym;
  Elf32_Word xndx;
  char *where;
} GElf_SymX;

static GElf_Word symsec_type = SHT_SYMTAB;


/* Maximum size of memory we allocate on the stack.  */
#define MAX_STACK_ALLOC	65536

/* Do elf_strptr, but return a backup string and never NULL.  */
static const char *
sym_name (Elf *elf, GElf_Word strndx, GElf_Word st_name, char buf[], size_t n)
{
  const char *symstr = elf_strptr (elf, strndx, st_name);
  if (symstr == NULL)
    {
      /*snprintf (buf, n, "[invalid st_name %#" PRIx32 "]", st_name);*/
      symstr = buf;
    }
  return symstr;
}

static void
show_symbols_bsd (Elf *elf, GElf_Word strndx,
		  const char *prefix, const char *fname, const char *fullname,
		  GElf_SymX *syms, size_t nsyms)
{
  /* Iterate over all symbols.  */
  for (size_t cnt = 0; cnt < nsyms; ++cnt)
    {
      char symstrbuf[50];
      const char *symstr = sym_name (elf, strndx, syms[cnt].sym.st_name,
				     symstrbuf, sizeof symstrbuf);

      if (symstr[0] == '\0')
	continue;

      if (syms[cnt].sym.st_shndx == SHN_UNDEF)
      {
          std::cout << symstr << std::endl;
      }
      else
      {
            std::cout << symstr << std::endl;
          //if( symstr[0] != '_' )
          //{
            //std::cout << symstr << std::endl;
          //}
          //else
          //{
            //std::cout << __cxxabiv1::__cxa_demangle(symstr, 0, 0, 0) << std::endl;
          //}
          //std::cout << symstr << std::endl;
      }
      //std::cout << syms[cnt].sym.st_value << std::endl;
    }
}

static void
show_symbols (Elf *elf, GElf_Ehdr *ehdr, Elf_Scn *scn, Elf_Scn *xndxscn,
	      GElf_Shdr *shdr, const char *prefix, const char *fname,
	      const char *fullname)
{
  /* Get the section header string table index.  */
  size_t shstrndx;

  if (elf_getshstrndx(elf, &shstrndx) < 0)
    error (EXIT_FAILURE, 0,
       gettext ("cannot get section header string table index"));

  /* The section is that large.  */
  size_t size = shdr->sh_size;
  /* One entry is this large.  */
  size_t entsize = shdr->sh_entsize;

  /* Consistency checks.  */
  if (entsize != gelf_fsize (elf, ELF_T_SYM, 1, ehdr->e_version))
    error (0, 0,
	   gettext ("%s: entry size in section `%s' is not what we expect"),
	   fullname, elf_strptr (elf, shstrndx, shdr->sh_name));
  else if (size % entsize != 0)
    error (0, 0,
	   gettext ("%s: size of section `%s' is not multiple of entry size"),
	   fullname, elf_strptr (elf, shstrndx, shdr->sh_name));

  /* Compute number of entries.  Handle buggy entsize values.  */
  size_t nentries = size / (entsize ?: 1);

  GElf_SymX *sym_mem;
  //if (nentries * sizeof (GElf_SymX) < MAX_STACK_ALLOC)
    //sym_mem = (GElf_SymX *) alloca (nentries * sizeof (GElf_SymX));
  //else
    sym_mem = (GElf_SymX *) malloc (nentries * sizeof (GElf_SymX));

  /* Get the data of the section.  */
  Elf_Data *data = elf_getdata (scn, NULL);
  Elf_Data *xndxdata = elf_getdata (xndxscn, NULL);
  if (data == NULL || (xndxscn != NULL && xndxdata == NULL));
    /*error ("%s",fullname);*/

  /* Iterate over all symbols.  */
  size_t nentries_used = 0;
  for (size_t cnt = 0; cnt < nentries; ++cnt)
    {
      //GElf_Sym *sym =
          gelf_getsymshndx (data, xndxdata, cnt,
					&sym_mem[nentries_used].sym,
					&sym_mem[nentries_used].xndx);
      /*if (sym == NULL)*/
	/*INTERNAL_ERROR (fullname);*/

      /* Filter out administrative symbols without a name and those
	 deselected by ther user with command line options.  */
      //sym_mem[nentries_used].where = ( char*)"";
           /* We use this entry.  */
      ++nentries_used;
    }
  /* Now we know the exact number.  */
  //nentries = nentries_used;


  /* Finally print according to the users selection.  */
      show_symbols_bsd (elf, shdr->sh_link, prefix, fname, fullname,
			sym_mem, nentries);

  /* Free all memory.  */
  if (nentries * sizeof (GElf_Sym) >= MAX_STACK_ALLOC)
    free (sym_mem);

}

static int
handle_elf (Elf *elf, const char *prefix, const char *fname,
	    const char *suffix)
{
  size_t prefix_len = prefix == NULL ? 0 : strlen (prefix);
  size_t suffix_len = suffix == NULL ? 0 : strlen (suffix);
  size_t fname_len = strlen (fname) + 1;
  char fullname[prefix_len + 1 + fname_len + suffix_len];
  char *cp = fullname;
  Elf_Scn *scn = NULL;
  int any = 0;
  int result = 0;
  GElf_Ehdr ehdr_mem;
  GElf_Ehdr *ehdr;

  /* We need the ELF header in a few places.  */
  ehdr = gelf_getehdr (elf, &ehdr_mem);
  /*if (ehdr == NULL)*/
    /*INTERNAL_ERROR (fullname);*/

  /* If we are asked to print the dynamic symbol table and this is
     executable or dynamic executable, fail.  */
/*  if (symsec_type == SHT_DYNSYM*/
      //&& ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN)
    //{
      //[> XXX Add machine specific object file types.  <]
      //error (0, 0, gettext ("%s%s%s%s: Invalid operation"),
		 //prefix ?: "", prefix ? "(" : "", fname, prefix ? ")" : "");
      //result = 1;
      //goto out;
    //}

  /* Create the full name of the file.  */
  if (prefix != NULL)
    cp = (char*)memcpy (cp, prefix, prefix_len);
  cp = (char *)memcpy (cp, fname, fname_len);

  if (suffix != NULL)
    memcpy (cp - 1, suffix, suffix_len + 1);

  while ((scn = elf_nextscn (elf, scn)) != NULL)
    {
      GElf_Shdr shdr_mem;
      GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);

      /*if (shdr == NULL)*/
	/*INTERNAL_ERROR (fullname);*/

      if (shdr->sh_type == symsec_type)
	{
	  Elf_Scn *xndxscn = NULL;

	  /* We have a symbol table.  First make sure we remember this.  */
	  any = 1;

	  /* Look for an extended section index table for this section.  */
	  //if (symsec_type == SHT_SYMTAB)
	    {
	      size_t scnndx = elf_ndxscn (scn);

	      while ((xndxscn = elf_nextscn (elf, xndxscn)) != NULL)
		{
		  GElf_Shdr xndxshdr_mem;
		  GElf_Shdr *xndxshdr = gelf_getshdr (xndxscn, &xndxshdr_mem);

		  /*if (xndxshdr == NULL)*/
			/*INTERNAL_ERROR (fullname);*/

		  if (xndxshdr->sh_type == SHT_SYMTAB_SHNDX
		      && xndxshdr->sh_link == scnndx)
		    break;
		}
	    }

	  show_symbols (elf, ehdr, scn, xndxscn, shdr, prefix, fname,
			fullname);
	}
    }

  if (! any)
    {
      error (0, 0, gettext ("%s%s%s: no symbols"),
	     prefix ?: "", prefix ? ":" : "", fname);
      result = 1;
    }

 //out:

  return result;
}

/* Open the file and determine the type.  */
static int
process_file (const char *fname, bool more_than_one)
{
  /* Open the file.  */
  int fd = open (fname, O_RDONLY);
  if (fd == -1)
    {
      error (0, errno, gettext ("cannot open '%s'"), fname);
      return 1;
    }

  /* Now get the ELF descriptor.  */
  Elf *elf = elf_begin (fd, ELF_C_READ_MMAP, NULL);
  if (elf != NULL)
    {
      if (elf_kind (elf) == ELF_K_ELF)
	{
	  int result = handle_elf (elf, more_than_one ? "" : NULL,
				   fname, NULL);

	  /*if (elf_end (elf) != 0)*/
		/*INTERNAL_ERROR (fname);*/

	  if (close (fd) != 0)
	    error (EXIT_FAILURE, errno, gettext ("while closing '%s'"), fname);

	  return result;
	}

      /* We cannot handle this type.  Close the descriptor anyway.  */
      /*if (elf_end (elf) != 0)*/
	/*INTERNAL_ERROR (fname);*/
    }

  error (0, 0, gettext ("%s: File format not recognized"), fname);

  return 1;
}


int main ()
{
  int result = 0;

  (void) elf_version (EV_CURRENT);

    result = process_file ("./main", false);

  return result;
}

