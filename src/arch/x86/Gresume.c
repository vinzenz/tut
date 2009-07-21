#include <stdlib.h>

#include "unwind_i.h"

HIDDEN inline int
x86_local_resume (unw_addr_space_t as, unw_cursor_t *cursor, void *arg)
{
#if defined(__linux)
  struct cursor *c = (struct cursor *) cursor;
  ucontext_t *uc = c->uc;

  /* Ensure c->pi is up-to-date.  On x86, it's relatively common to be
     missing DWARF unwind info.  We don't want to fail in that case,
     because the frame-chain still would let us do a backtrace at
     least.  */
  dwarf_make_proc_info (&c->dwarf);

  if (unlikely (c->sigcontext_format != X86_SCF_NONE))
    {
      struct sigcontext *sc = (struct sigcontext *) c->sigcontext_addr;

      Debug (8, "resuming at ip=%x via sigreturn(%p)\n", c->dwarf.ip, sc);
      sigreturn (sc);
    }
  else
    {
      Debug (8, "resuming at ip=%x via setcontext()\n", c->dwarf.ip);
      setcontext (uc);
    }
#endif
  return -UNW_EINVAL;
}

/* This routine is responsible for copying the register values in
   cursor C and establishing them as the current machine state. */

static inline int
establish_machine_state (struct cursor *c)
{
  int (*access_reg) (unw_addr_space_t, unw_regnum_t, unw_word_t *,
		     int write, void *);
  int (*access_fpreg) (unw_addr_space_t, unw_regnum_t, unw_fpreg_t *,
		       int write, void *);
  unw_addr_space_t as = c->dwarf.as;
  void *arg = c->dwarf.as_arg;
  unw_fpreg_t fpval;
  unw_word_t val;
  int reg;

  access_reg = as->acc.access_reg;
  access_fpreg = as->acc.access_fpreg;

  Debug (8, "copying out cursor state\n");

  for (reg = 0; reg <= UNW_REG_LAST; ++reg)
    {
      Debug (16, "copying %s %d\n", unw_regname (reg), reg);
      if (unw_is_fpreg (reg))
	{
	  if (tdep_access_fpreg (c, reg, &fpval, 0) >= 0)
	    (*access_fpreg) (as, reg, &fpval, 1, arg);
	}
      else
	{
	  if (tdep_access_reg (c, reg, &val, 0) >= 0)
	    (*access_reg) (as, reg, &val, 1, arg);
	}
    }
  return 0;
}

PROTECTED int
unw_resume (unw_cursor_t *cursor)
{
  struct cursor *c = (struct cursor *) cursor;
  int ret;

  Debug (1, "(cursor=%p)\n", c);

  if ((ret = establish_machine_state (c)) < 0)
    return ret;

  return (*c->dwarf.as->acc.resume) (c->dwarf.as, (unw_cursor_t *) c,
				     c->dwarf.as_arg);
}
