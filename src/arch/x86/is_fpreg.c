#include "libunwind_i.h"

PROTECTED int
unw_is_fpreg (int regnum)
{
  return ((regnum >= UNW_X86_ST0 && regnum <= UNW_X86_ST7)
	  || (regnum >= UNW_X86_XMM0_lo && regnum <= UNW_X86_XMM7_hi)
	  || (regnum >= UNW_X86_XMM0 && regnum <= UNW_X86_XMM7));
}
