#include <stdio.h>
#include <libunwind.h>

void unwind_to_prev_frame(void)
{
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

void f3(int a)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    unwind_to_prev_frame();

    printf("\nTHIS TEXT SHOULD BE OMITTED\n");
}

void f2(int a, const char* b)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    f3(a+1);
    printf("\nTHIS TEXT SHOULD BE WRITTEN\n");
}

void f1()
{
    printf("%s\n", __PRETTY_FUNCTION__);
    f2(1, __PRETTY_FUNCTION__);
}

int main()
{
    f1();    

    return 0;
}
