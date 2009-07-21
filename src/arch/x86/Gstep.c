#include "unwind_i.h"
//#include "offsets.h"

PROTECTED int
unw_step (unw_cursor_t *cursor)
{
  struct cursor *c = (struct cursor *) cursor;
  int ret, i;

  Debug (1, "(cursor=%p, ip=0x%08x)\n", c, (unsigned) c->dwarf.ip);

  /* Try DWARF-based unwinding... */
    ret = dwarf_step (&c->dwarf);
  Debug (2, "returning %d\n", ret);
    return ret;
/*
  if (ret < 0 && ret != -UNW_ENOINFO)
    {
      Debug (2, "returning %d\n", ret);
      return ret;
    }

  if (unlikely (ret < 0))
    {
      struct dwarf_loc ebp_loc, eip_loc;

      c->validate = 1;

      Debug (13, "dwarf_step() failed (ret=%d), trying frame-chain\n", ret);

      if (unw_is_signal_frame (cursor))
	{
	  unw_word_t sc_addr;
	  unw_word_t siginfo_ptr_addr = c->dwarf.cfa + 4;
	  unw_word_t sigcontext_ptr_addr = c->dwarf.cfa + 8;
	  unw_word_t siginfo_ptr, sigcontext_ptr;
	  struct dwarf_loc esp_loc, siginfo_ptr_loc, sigcontext_ptr_loc;

	  siginfo_ptr_loc = DWARF_LOC (siginfo_ptr_addr, 0);
	  sigcontext_ptr_loc = DWARF_LOC (sigcontext_ptr_addr, 0);
	  ret = (dwarf_get (&c->dwarf, siginfo_ptr_loc, &siginfo_ptr)
		 | dwarf_get (&c->dwarf, sigcontext_ptr_loc, &sigcontext_ptr));
	  if (ret < 0)
	    {
	      Debug (2, "returning 0\n");
	      return 0;
	    }
	  if (siginfo_ptr < c->dwarf.cfa
	      || siginfo_ptr > c->dwarf.cfa + 256
	      || sigcontext_ptr < c->dwarf.cfa
	      || sigcontext_ptr > c->dwarf.cfa + 256)
	    {
	      c->sigcontext_format = X86_SCF_LINUX_SIGFRAME;
	      c->sigcontext_addr = sc_addr = c->dwarf.cfa + 4;
	    }
	  else
	    {
	      c->sigcontext_format = X86_SCF_LINUX_RT_SIGFRAME;
	      c->sigcontext_addr = sigcontext_ptr;
	      sc_addr = sigcontext_ptr + LINUX_UC_MCONTEXT_OFF;
	    }
	  esp_loc = DWARF_LOC (sc_addr + LINUX_SC_ESP_OFF, 0);
	  ebp_loc = DWARF_LOC (sc_addr + LINUX_SC_EBP_OFF, 0);
	  eip_loc = DWARF_LOC (sc_addr + LINUX_SC_EIP_OFF, 0);
	  ret = dwarf_get (&c->dwarf, esp_loc, &c->dwarf.cfa);
	  if (ret < 0)
	    {
	      Debug (2, "returning 0\n");
	      return 0;
	    }

	  c->dwarf.loc[EAX] = DWARF_LOC (sc_addr + LINUX_SC_EAX_OFF, 0);
	  c->dwarf.loc[ECX] = DWARF_LOC (sc_addr + LINUX_SC_ECX_OFF, 0);
	  c->dwarf.loc[EDX] = DWARF_LOC (sc_addr + LINUX_SC_EDX_OFF, 0);
	  c->dwarf.loc[EBX] = DWARF_LOC (sc_addr + LINUX_SC_EBX_OFF, 0);
	  c->dwarf.loc[EBP] = DWARF_LOC (sc_addr + LINUX_SC_EBP_OFF, 0);
	  c->dwarf.loc[ESI] = DWARF_LOC (sc_addr + LINUX_SC_ESI_OFF, 0);
	  c->dwarf.loc[EDI] = DWARF_LOC (sc_addr + LINUX_SC_EDI_OFF, 0);
	  c->dwarf.loc[EFLAGS] = DWARF_NULL_LOC;
	  c->dwarf.loc[TRAPNO] = DWARF_NULL_LOC;
	  c->dwarf.loc[ST0] = DWARF_NULL_LOC;
	}
      else
	{
	  ret = dwarf_get (&c->dwarf, c->dwarf.loc[EBP], &c->dwarf.cfa);
	  if (ret < 0)
	    {
	      Debug (2, "returning %d\n", ret);
	      return ret;
	    }

	  Debug (13, "[EBP=0x%x] = 0x%x\n", DWARF_GET_LOC (c->dwarf.loc[EBP]),
		 c->dwarf.cfa);

	  ebp_loc = DWARF_LOC (c->dwarf.cfa, 0);
	  eip_loc = DWARF_LOC (c->dwarf.cfa + 4, 0);
	  c->dwarf.cfa += 8;

	  for (i = 0; i < DWARF_NUM_PRESERVED_REGS; ++i)
	    c->dwarf.loc[i] = DWARF_NULL_LOC;
	}
      c->dwarf.loc[EBP] = ebp_loc;
      c->dwarf.loc[EIP] = eip_loc;
      c->dwarf.ret_addr_column = EIP;

      if (!DWARF_IS_NULL_LOC (c->dwarf.loc[EBP]))
	{
	  ret = dwarf_get (&c->dwarf, c->dwarf.loc[EIP], &c->dwarf.ip);
	  if (ret < 0)
	    {
	      Debug (2, "returning %d\n", ret);
	      return ret;
	    }
	}
      else
	c->dwarf.ip = 0;
    }
  ret = (c->dwarf.ip == 0) ? 0 : 1;
  Debug (2, "returning %d\n", ret);
  return ret;*/
}
