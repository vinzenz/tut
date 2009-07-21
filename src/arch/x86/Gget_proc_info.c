#include "unwind_i.h"

PROTECTED int
unw_get_proc_info (unw_cursor_t *cursor, unw_proc_info_t *pi)
{
  struct cursor *c = (struct cursor *) cursor;

  if (dwarf_make_proc_info (&c->dwarf) < 0)
    {
      /* On x86, it's relatively common to be missing DWARF unwind
	 info.  We don't want to fail in that case, because the
	 frame-chain still would let us do a backtrace at least.  */
      memset (pi, 0, sizeof (*pi));
      pi->start_ip = c->dwarf.ip;
      pi->end_ip = c->dwarf.ip + 1;
      return 0;
    }
  *pi = c->dwarf.pi;
  return 0;
}
