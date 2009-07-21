#include "dwarf_i.h"
#include "libunwind_i.h"

#include <assert.h>

HIDDEN int
dwarf_read_encoded_pointer (unw_addr_space_t as, unw_accessors_t *a,
			    unw_word_t *addr, unsigned char encoding,
			    const unw_proc_info_t *pi,
			    unw_word_t *valp, void *arg)
{
  return dwarf_read_encoded_pointer_inlined (as, a, addr, encoding,
					     pi, valp, arg);
}
