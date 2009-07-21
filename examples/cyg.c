#include <stdio.h>
#include <libunwind.h>

void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));

void* ptr = NULL;

void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
    if (this_fn == ptr)
    {
        //get parameters using dwaf
        //run function in c++ using parameters
        f3_2(0);

        //unwind to prev frame

        unw_cursor_t cursor, dd; unw_context_t uc;
        unw_word_t ip, sp;
    
        unw_getcontext(&uc);
        unw_init_local(&cursor, &uc);

        unsigned int i=0;
  
        while (unw_step(&cursor) > 0)
        {
            if (++i == 2)//prev 1->this function frame stack, 2->prev frame stack, ...
            {
                unw_resume(&cursor); //nice and simple :]
            }
        }
    }
}

void f3_1(int a)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    printf("\nTHIS TEXT SHOULD BE OMITTED\n");
}

void f3_2(int a)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    printf("\nSIMPEL STUP\n");
}

void f2(int a, const char* b)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    f3_1(a+1);
    printf("\nTHIS TEXT SHOULD BE WRITTEN\n");
}

void f1()
{
    printf("%s\n", __PRETTY_FUNCTION__);
    f2(1, __PRETTY_FUNCTION__);
}

int main()
{
    ptr = &f3_1;
    f1();    

    return 0;
}
