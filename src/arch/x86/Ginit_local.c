#include "unwind_i.h"
#include "init.h"

PROTECTED int
unw_init_local (unw_cursor_t *cursor, ucontext_t *uc)
{
  struct cursor *c = (struct cursor *) cursor;

  if (tdep_needs_initialization)
    tdep_init ();

  Debug (1, "(cursor=%p)\n", c);

  c->dwarf.as = unw_local_addr_space;
  c->dwarf.as_arg = c;
  c->uc = uc;
  c->validate = 0;
  return common_init (c);
}
