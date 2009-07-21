#include "dwarf.h"
#include "libunwind_i.h"

HIDDEN int
dwarf_step (struct dwarf_cursor *c)
{
  unw_word_t prev_cfa = c->cfa;
  int ret;

  if ((ret = dwarf_find_save_locs (c)) >= 0) {
    c->pi_valid = 0;
    ret = (c->ip == 0) ? 0 : 1;
  }

  Debug (15, "returning %d\n", ret);
  return ret;
}
