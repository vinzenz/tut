#include "unwind_i.h"
#include "dwarf_i.h"

HIDDEN pthread_mutex_t x86_lock = PTHREAD_MUTEX_INITIALIZER;
HIDDEN int tdep_needs_initialization = 1;

/* See comments for svr4_dbx_register_map[] in gcc/config/i386/i386.c.  */

HIDDEN uint8_t dwarf_to_unw_regnum_map[19] =
  {
    UNW_X86_EAX, UNW_X86_ECX, UNW_X86_EDX, UNW_X86_EBX,
    UNW_X86_ESP, UNW_X86_EBP, UNW_X86_ESI, UNW_X86_EDI,
    UNW_X86_EIP, UNW_X86_EFLAGS, UNW_X86_TRAPNO,
    UNW_X86_ST0, UNW_X86_ST1, UNW_X86_ST2, UNW_X86_ST3,
    UNW_X86_ST4, UNW_X86_ST5, UNW_X86_ST6, UNW_X86_ST7
  };

HIDDEN void
tdep_init (void)
{
  intrmask_t saved_mask;

  sigfillset (&unwi_full_mask);

  sigprocmask (SIG_SETMASK, &unwi_full_mask, &saved_mask);
  mutex_lock (&x86_lock);
  {
    if (!tdep_needs_initialization)
      /* another thread else beat us to it... */
      goto out;

    mi_init ();

    dwarf_init ();

    x86_local_addr_space_init ();
    tdep_needs_initialization = 0;	/* signal that we're initialized... */
  }
 out:
  mutex_unlock (&x86_lock);
  sigprocmask (SIG_SETMASK, &saved_mask, NULL);
}
