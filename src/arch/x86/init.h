#include "unwind_i.h"

static inline int
common_init (struct cursor *c)
{
  int ret, i;

  c->dwarf.loc[EAX] = DWARF_REG_LOC (&c->dwarf, UNW_X86_EAX);
  c->dwarf.loc[ECX] = DWARF_REG_LOC (&c->dwarf, UNW_X86_ECX);
  c->dwarf.loc[EDX] = DWARF_REG_LOC (&c->dwarf, UNW_X86_EDX);
  c->dwarf.loc[EBX] = DWARF_REG_LOC (&c->dwarf, UNW_X86_EBX);
  c->dwarf.loc[ESP] = DWARF_REG_LOC (&c->dwarf, UNW_X86_ESP);
  c->dwarf.loc[EBP] = DWARF_REG_LOC (&c->dwarf, UNW_X86_EBP);
  c->dwarf.loc[ESI] = DWARF_REG_LOC (&c->dwarf, UNW_X86_ESI);
  c->dwarf.loc[EDI] = DWARF_REG_LOC (&c->dwarf, UNW_X86_EDI);
  c->dwarf.loc[EIP] = DWARF_REG_LOC (&c->dwarf, UNW_X86_EIP);
  c->dwarf.loc[EFLAGS] = DWARF_REG_LOC (&c->dwarf, UNW_X86_EFLAGS);
  c->dwarf.loc[TRAPNO] = DWARF_REG_LOC (&c->dwarf, UNW_X86_TRAPNO);
  c->dwarf.loc[ST0] = DWARF_REG_LOC (&c->dwarf, UNW_X86_ST0);
  for (i = ST0 + 1; i < DWARF_NUM_PRESERVED_REGS; ++i)
    c->dwarf.loc[i] = DWARF_NULL_LOC;

  ret = dwarf_get (&c->dwarf, c->dwarf.loc[EIP], &c->dwarf.ip);
  if (ret < 0)
    return ret;

  ret = dwarf_get (&c->dwarf, DWARF_REG_LOC (&c->dwarf, UNW_X86_ESP),
		   &c->dwarf.cfa);
  if (ret < 0)
    return ret;

  c->sigcontext_format = X86_SCF_NONE;
  c->sigcontext_addr = 0;

  c->dwarf.args_size = 0;
  c->dwarf.ret_addr_column = 0;
  c->dwarf.pi_valid = 0;
  c->dwarf.pi_is_dynamic = 0;
  c->dwarf.hint = 0;
  c->dwarf.prev_rs = 0;

  return 0;
}
