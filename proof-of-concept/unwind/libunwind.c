#include <stddef.h>
#include <inttypes.h>
#include <ucontext.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <link.h> //for elf
#include <dwarf.h>

#define Debug(level,format...)\
do {\
	int _n = 16;\
      fprintf (stderr, "%*c>%s: ", _n, ' ', __FUNCTION__);\
      fprintf (stderr, format);\
} while (0)

enum regs
{
    EAX,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI,
    EIP
};

typedef enum
  {
    UNW_X86_EAX,	/* scratch (exception argument 1) */
    UNW_X86_EDX,	/* scratch (exception argument 2) */
    UNW_X86_ECX,	/* scratch */
    UNW_X86_EBX,	/* preserved */
    UNW_X86_ESI,	/* preserved */
    UNW_X86_EDI,	/* preserved */
    UNW_X86_EBP,	/* (optional) frame-register */
    UNW_X86_ESP,	/* (optional) frame-register */
    UNW_X86_EIP,	/* frame-register */

    UNW_X86_CFA,

    UNW_TDEP_LAST_REG = UNW_X86_EIP,

    UNW_TDEP_IP = UNW_X86_EIP,
    UNW_TDEP_SP = UNW_X86_CFA,
    UNW_TDEP_EH = UNW_X86_EAX
  }
x86_regnum_t;

typedef enum
  {
    UNW_REG_IP = UNW_TDEP_IP,		/* (rw) instruction pointer (pc) */
    UNW_REG_SP = UNW_TDEP_SP,		/* (ro) stack pointer */
    UNW_REG_EH = UNW_TDEP_EH,		/* (rw) exception-handling reg base */
    UNW_REG_LAST = UNW_TDEP_LAST_REG
  }
unw_frame_regnum_t;

#define DW_EH_PE_FORMAT_MASK	0x0f	/* format of the encoded value */
#define DW_EH_PE_APPL_MASK	0x70	/* how the value is to be applied */
#define DW_EH_PE_indirect	0x80
#define DW_EH_PE_omit		0xff
#define DW_EH_PE_ptr		0x00	/* pointer-sized unsigned value */
#define DW_EH_PE_uleb128	0x01	/* unsigned LE base-128 value */
#define DW_EH_PE_udata2		0x02	/* unsigned 16-bit value */
#define DW_EH_PE_udata4		0x03	/* unsigned 32-bit value */
#define DW_EH_PE_udata8		0x04	/* unsigned 64-bit value */
#define DW_EH_PE_sleb128	0x09	/* signed LE base-128 value */
#define DW_EH_PE_sdata2		0x0a	/* signed 16-bit value */
#define DW_EH_PE_sdata4		0x0b	/* signed 32-bit value */
#define DW_EH_PE_sdata8		0x0c	/* signed 64-bit value */
#define DW_EH_PE_absptr		0x00	/* absolute value */
#define DW_EH_PE_pcrel		0x10	/* rel. to addr. of encoded value */
#define DW_EH_PE_textrel	0x20	/* text-relative (GCC-specific???) */
#define DW_EH_PE_datarel	0x30	/* data-relative */
#define DW_EH_PE_funcrel	0x40	/* start-of-procedure-relative */
#define DW_EH_PE_aligned	0x50	/* aligned pointer */

#define UNW_TDEP_CURSOR_LEN	127
#define UNW_TDEP_NUM_EH_REGS	2	/* eax and edx are exception args */
#define DWARF_CIE_VERSION	3	/* GCC emits version 1??? */
#define DW_EH_VERSION		1	/* The version we're implementing */

#define DWARF_GET_LOC(l)	((l).val)
#define DWARF_NULL_LOC		DWARF_LOC (0, 0)
#define DWARF_IS_NULL_LOC(l)	(DWARF_GET_LOC (l) == 0)
#define DWARF_LOC(r, t)	((dwarf_loc_t) { r })
#define DWARF_IS_REG_LOC(l)	0
#define DWARF_REG_LOC(c,r)     (DWARF_LOC((unw_word_t) tdep_uc_addr(dwarf_get_uc(c), (r)), 0))
#define DWARF_MEM_LOC(c,m)	DWARF_LOC ((m), 0)

#define DWARF_NUM_PRESERVED_REGS	17
#define DWARF_REGNUM_MAP_LENGTH		19
#define DWARF_CFA_REG_COLUMN	DWARF_NUM_PRESERVED_REGS
#define DWARF_CFA_OFF_COLUMN	(DWARF_NUM_PRESERVED_REGS + 1)
#define DWARF_CFA_OPCODE_MASK	0xc0
#define DWARF_CFA_OPERAND_MASK	0x3f

typedef unsigned long unw_word_t;
typedef long unw_sword_t;
typedef struct unw_addr_space *unw_addr_space_t;
typedef unsigned char unw_hash_index_t;
typedef ucontext_t unw_tdep_context_t;
typedef int unw_regnum_t;
typedef unw_tdep_context_t unw_context_t;

struct dwarf_eh_frame_hdr
  {
    unsigned char version;
    unsigned char eh_frame_ptr_enc;
    unsigned char fde_count_enc;
    unsigned char table_enc;
  };

typedef struct unw_cursor
  {
    unw_word_t opaque[UNW_TDEP_CURSOR_LEN];
  }
unw_cursor_t;

typedef enum
  {
    DWARF_WHERE_UNDEF,		/* register isn't saved at all */
    DWARF_WHERE_SAME,		/* register has same value as in prev. frame */
    DWARF_WHERE_CFAREL,		/* register saved at CFA-relative address */
    DWARF_WHERE_REG,		/* register saved in another register */
    DWARF_WHERE_EXPR,		/* register saved */
  }
dwarf_where_t;

typedef struct
  {
    dwarf_where_t where;	/* how is the register saved? */
    unw_word_t val;		/* where it's saved */
  }
dwarf_save_loc_t;

typedef struct dwarf_reg_state
  {
    struct dwarf_reg_state *next;	/* for rs_stack */
    dwarf_save_loc_t reg[DWARF_NUM_PRESERVED_REGS + 2];
    unw_word_t ip;		          /* ip this rs is for */
    unw_word_t ret_addr_column;           /* indicates which column in the rule table represents return address */
  }
dwarf_reg_state_t;


typedef struct dwarf_loc
  {
    unw_word_t val;
  }
dwarf_loc_t;

typedef struct unw_proc_info
  {
    unw_word_t start_ip;	/* first IP covered by this procedure */
    unw_word_t end_ip;		/* first IP NOT covered by this procedure */
    unw_word_t lsda;		/* address of lang.-spec. data area (if any) */
    unw_word_t handler;		/* optional personality routine */
    unw_word_t gp;		/* global-pointer value for this procedure */
    unw_word_t flags;		/* misc. flags */

    int unwind_info_size;	/* size of the information (if applicable) */
    void *unwind_info;		/* unwind-info (arch-specific) */
  }
unw_proc_info_t;

typedef struct dwarf_cursor
  {
    void *as_arg;		/* argument to address-space callbacks */

    unw_word_t cfa;	/* canonical frame address; aka frame-/stack-pointer */
    unw_word_t ip;		/* instruction pointer */
    unw_word_t ret_addr_column;	/* column for return-address */

    unw_word_t eh_args[UNW_TDEP_NUM_EH_REGS];
    unsigned int eh_valid_mask;

    dwarf_loc_t loc[DWARF_NUM_PRESERVED_REGS];

    unw_proc_info_t pi;		/* info about current procedure */
  }
dwarf_cursor_t;

struct cursor
  {
    struct dwarf_cursor dwarf;		/* must be first */
    ucontext_t *uc;
  };

typedef union
  {
    int8_t s8;
    int16_t s16;
    int32_t s32;
    int64_t s64;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    unw_word_t w;
    void *ptr;
  }
  dwarf_misaligned_value_t;

  struct table_entry
  {
    int32_t start_ip_offset;
    int32_t fde_offset;
  };

typedef struct dwarf_cie_info
  {
    unw_word_t cie_instr_start;	/* start addr. of CIE "initial_instructions" */
    unw_word_t cie_instr_end;	/* end addr. of CIE "initial_instructions" */
    unw_word_t fde_instr_start;	/* start addr. of FDE "instructions" */
    unw_word_t fde_instr_end;	/* end addr. of FDE "instructions" */
    unw_word_t code_align;	/* code-alignment factor */
    unw_word_t data_align;	/* data-alignment factor */
    unw_word_t ret_addr_column;	/* column of return-address register */
    unw_word_t handler;		/* address of personality-routine */
    uint16_t abi;
    uint16_t tag;
    uint8_t fde_encoding;
    uint8_t lsda_encoding;
    unsigned int sized_augmentation : 1;
    unsigned int have_abi_marker : 1;
  }
dwarf_cie_info_t;

typedef struct dwarf_state_record
  {
    unsigned char fde_encoding;
    unw_word_t args_size;

    dwarf_reg_state_t rs_initial;	/* reg-state after CIE instructions */
    dwarf_reg_state_t rs_current;	/* current reg-state */
  }
dwarf_state_record_t;

typedef struct unw_dyn_remote_table_info
  {
    unw_word_t name_ptr;	/* addr. of table name (e.g., library name) */
    unw_word_t segbase;		/* segment base */
    unw_word_t table_len;	/* must be a multiple of sizeof(unw_word_t)! */
    unw_word_t table_data;
  }
unw_dyn_remote_table_info_t;

typedef struct unw_dyn_info
  {
    /* doubly-linked list of dyn-info structures: */
    struct unw_dyn_info *next;
    struct unw_dyn_info *prev;
    unw_word_t start_ip;	/* first IP covered by this entry */
    unw_word_t end_ip;		/* first IP NOT covered by this entry */
    unw_word_t gp;		/* global-pointer in effect for this entry */
    int32_t format;		/* real type: unw_dyn_info_format_t */
    int32_t pad;
    union
      {
	unw_dyn_remote_table_info_t rti;
      }
    u;
  }
unw_dyn_info_t;

static uint8_t dwarf_to_unw_regnum_map[19] =
  {
    UNW_X86_EAX, UNW_X86_ECX, UNW_X86_EDX, UNW_X86_EBX,
    UNW_X86_ESP, UNW_X86_EBP, UNW_X86_ESI, UNW_X86_EDI,
    UNW_X86_EIP
  };

struct callback_data
  {
    /* in: */
    unw_word_t ip;		/* instruction-pointer we're looking for */
    unw_proc_info_t *pi;	/* proc-info pointer */
    int need_unwind_info;
    /* out: */
    int single_fde;		/* did we find a single FDE? (vs. a table) */
    unw_dyn_info_t di;		/* table info (if single_fde is false) */
  };

int dwarf_reads8 (unw_word_t *addr, int8_t *val, void *arg)
{
  dwarf_misaligned_value_t *mvp = (dwarf_misaligned_value_t *) *addr;

  *val = mvp->s8;
  *addr += sizeof (mvp->s8);
  return 0;
}

int dwarf_reads16 (unw_word_t *addr, int16_t *val, void *arg)
{
  dwarf_misaligned_value_t *mvp = (dwarf_misaligned_value_t *) *addr;

  *val = mvp->s16;
  *addr += sizeof (mvp->s16);
  return 0;
}

int dwarf_reads32 (unw_word_t *addr, int32_t *val, void *arg)
{
  dwarf_misaligned_value_t *mvp = (dwarf_misaligned_value_t *) *addr;

  *val = mvp->s32;
  *addr += sizeof (mvp->s32);
  return 0;
}

int dwarf_reads64 (unw_word_t *addr, int64_t *val, void *arg)
{
  dwarf_misaligned_value_t *mvp = (dwarf_misaligned_value_t *) *addr;

  *val = mvp->s64;
  *addr += sizeof (mvp->s64);
  return 0;
}

int dwarf_readu8 (unw_word_t *addr, uint8_t *val, void *arg)
{
  dwarf_misaligned_value_t *mvp = (dwarf_misaligned_value_t *) *addr;

  *val = mvp->u8;
  *addr += sizeof (mvp->u8);
  return 0;
}

int dwarf_readu16 (unw_word_t *addr, uint16_t *val, void *arg)
{
  dwarf_misaligned_value_t *mvp = (dwarf_misaligned_value_t *) *addr;

  *val = mvp->u16;
  *addr += sizeof (mvp->u16);
  return 0;
}

int dwarf_readu32 (unw_word_t *addr, uint32_t *val, void *arg)
{
  dwarf_misaligned_value_t *mvp = (dwarf_misaligned_value_t *) *addr;

  *val = mvp->u32;
  *addr += sizeof (mvp->u32);
  return 0;
}

static inline int
dwarf_readu64 (unw_word_t *addr, uint64_t *val, void *arg)
{
  dwarf_misaligned_value_t *mvp = (dwarf_misaligned_value_t *) *addr;

  *val = mvp->u64;
  *addr += sizeof (mvp->u64);
  return 0;
}

int dwarf_readw (unw_word_t *addr, unw_word_t *val, void *arg)
{
  dwarf_misaligned_value_t *mvp = (dwarf_misaligned_value_t *) *addr;

  *val = mvp->w;
  *addr += sizeof (mvp->w);
  return 0;
}

int dwarf_read_uleb128 (unw_word_t *addr, unw_word_t *valp, void *arg)
{
  unw_word_t val = 0, shift = 0;
  unsigned char byte;
  int ret;

  do
    {
      if ((ret = dwarf_readu8 (addr, &byte, arg)) < 0)
	return ret;

      val |= ((unw_word_t) byte & 0x7f) << shift;
      shift += 7;
    }
  while (byte & 0x80);

  *valp = val;
  return 0;
}

int dwarf_read_sleb128 (unw_word_t *addr, unw_word_t *valp, void *arg)
{
  unw_word_t val = 0, shift = 0;
  unsigned char byte;
  int ret;

  do
    {
      if ((ret = dwarf_readu8 (addr, &byte, arg)) < 0)
      {
    	return ret;
      }

      val |= ((unw_word_t) byte & 0x7f) << shift;
      shift += 7;
    }
  while (byte & 0x80);

  if (shift < 8 * sizeof (unw_word_t) && (byte & 0x40) != 0)
    /* sign-extend negative value */
    val |= ((unw_word_t) -1) << shift;

  *valp = val;
  return 0;
}

static inline int
dwarf_read_encoded_pointer_inlined (
				    unw_word_t *addr, unsigned char encoding,
				    const unw_proc_info_t *pi,
				    unw_word_t *valp, void *arg)
{
  unw_word_t val, initial_addr = *addr;
  uint16_t uval16;
  uint32_t uval32;
  uint64_t uval64;
  int16_t sval16;
  int32_t sval32;
  int64_t sval64;
  int ret;

  /* DW_EH_PE_omit and DW_EH_PE_aligned don't follow the normal
     format/application encoding.  Handle them first.  */
  if (encoding == DW_EH_PE_omit)
    {
      *valp = 0;
      return 0;
    }
  else if (encoding == DW_EH_PE_aligned)
    {
      *addr = (initial_addr + sizeof (unw_word_t) - 1) & -sizeof (unw_word_t);
      return dwarf_readw (addr, valp, arg);
    }

  switch (encoding & DW_EH_PE_FORMAT_MASK)
    {
    case DW_EH_PE_ptr:
      if ((ret = dwarf_readw (addr, &val, arg)) < 0)
	return ret;
      break;

    case DW_EH_PE_uleb128:
      if ((ret = dwarf_read_uleb128 (addr, &val, arg)) < 0)
	return ret;
      break;

    case DW_EH_PE_udata2:
      if ((ret = dwarf_readu16 (addr, &uval16, arg)) < 0)
	return ret;
      val = uval16;
      break;

    case DW_EH_PE_udata4:
      if ((ret = dwarf_readu32 (addr, &uval32, arg)) < 0)
	return ret;
      val = uval32;
      break;

    case DW_EH_PE_udata8:
      if ((ret = dwarf_readu64 (addr, &uval64, arg)) < 0)
	return ret;
      val = uval64;
      break;

    case DW_EH_PE_sleb128:
      if ((ret = dwarf_read_uleb128 (addr, &val, arg)) < 0)
	return ret;
      break;

    case DW_EH_PE_sdata2:
      if ((ret = dwarf_reads16 (addr, &sval16, arg)) < 0)
	return ret;
      val = sval16;
      break;

    case DW_EH_PE_sdata4:
      if ((ret = dwarf_reads32 (addr, &sval32, arg)) < 0)
	return ret;
      val = sval32;
      break;

    case DW_EH_PE_sdata8:
      if ((ret = dwarf_reads64 (addr, &sval64, arg)) < 0)
	return ret;
      val = sval64;
      break;

    default:
      Debug (1, "unexpected encoding format 0x%x\n",
	     encoding & DW_EH_PE_FORMAT_MASK);
      return -1;
    }

  if (val == 0)
    {
      /* 0 is a special value and always absolute.  */
      *valp = 0;
      return 0;
    }

  switch (encoding & DW_EH_PE_APPL_MASK)
    {
    case DW_EH_PE_absptr:
      break;

    case DW_EH_PE_pcrel:
      val += initial_addr;
      break;

    case DW_EH_PE_datarel:
      /* XXX For now, assume that data-relative addresses are relative
         to the global pointer.  */
      val += pi->gp;
      break;

    case DW_EH_PE_funcrel:
      val += pi->start_ip;
      break;

    case DW_EH_PE_textrel:
      /* XXX For now we don't support text-rel values.  If there is a
         platform which needs this, we probably would have to add a
         "segbase" member to unw_proc_info_t.  */
    default:
      Debug (1, "unexpected application type 0x%x\n",
	     encoding & DW_EH_PE_APPL_MASK);
      return -1;
    }

  if (encoding & DW_EH_PE_indirect)
    {
      unw_word_t indirect_addr = val;

      if ((ret = dwarf_readw (&indirect_addr, &val, arg)) < 0)
	return ret;
    }

  *valp = val;
  return 0;
}

int dwarf_read_encoded_pointer ( unw_word_t *addr, unsigned char encoding, const unw_proc_info_t *pi, unw_word_t *valp, void *arg)
{
  return dwarf_read_encoded_pointer_inlined (addr, encoding,
					     pi, valp, arg);
}

ucontext_t *dwarf_get_uc(const struct dwarf_cursor *p_cursor)
{
    cursor *c = ( cursor *) p_cursor->as_arg;
    return c->uc;
}

int dwarf_get (struct dwarf_cursor *c, dwarf_loc_t loc, unw_word_t *val)
{
  if (!loc.val)
    return -1;
  *val = *(unw_word_t *) (loc.val);
  return 0;
}

int dwarf_put (struct dwarf_cursor *c, dwarf_loc_t loc, unw_word_t val)
{
  if (!loc.val)
    return -1;
  *(unw_word_t *) (loc.val) = val;
  return 0;
}

int dwarf_to_unw_regnum(int reg)
{
  return (((reg) <= DWARF_REGNUM_MAP_LENGTH) ? dwarf_to_unw_regnum_map[reg] : 0);
}

int is_cie_id (unw_word_t val)
{
  return (val == 0 || val == - (unw_word_t) 1);
}

int parse_cie (unw_word_t addr, const unw_proc_info_t *pi, struct dwarf_cie_info *dci, void *arg)
{
  uint8_t version, ch, augstr[5], fde_encoding, handler_encoding;
  unw_word_t len, cie_end_addr, aug_size;
  uint32_t u32val;
  uint64_t u64val;
  size_t i;
  int ret;

  switch (sizeof (unw_word_t))
    {
    case 4:	fde_encoding = DW_EH_PE_udata4; break;
    case 8:	fde_encoding = DW_EH_PE_udata8; break;
    default:	fde_encoding = DW_EH_PE_omit; break;
    }

  dci->lsda_encoding = DW_EH_PE_omit;
  dci->handler = 0;

  if ((ret = dwarf_readu32 (&addr, &u32val, arg)) < 0)
    return ret;

  if (u32val != 0xffffffff)
    {
      /* the CIE is in the 32-bit DWARF format */
      uint32_t cie_id;

      len = u32val;
      cie_end_addr = addr + len;
      if ((ret = dwarf_readu32 (&addr, &cie_id, arg)) < 0)
	return ret;
      /* DWARF says CIE id should be 0xffffffff, but in .eh_frame, it's 0 */
      if (cie_id != 0)
	{
	  Debug (1, "Unexpected CIE id %x\n", cie_id);
	  return -1;
	}
    }
  else
    {
      /* the CIE is in the 64-bit DWARF format */
      uint64_t cie_id;

      if ((ret = dwarf_readu64 (&addr, &u64val, arg)) < 0)
	return ret;
      len = u64val;
      cie_end_addr = addr + len;
      if ((ret = dwarf_readu64 (&addr, &cie_id, arg)) < 0)
	return ret;
      /* DWARF says CIE id should be 0xffffffffffffffff, but in
	 .eh_frame, it's 0 */
      if (cie_id != 0)
	{
	  Debug (1, "Unexpected CIE id %llx\n", (long long) cie_id);
	  return -1;
	}
    }
  dci->cie_instr_end = cie_end_addr;

  if ((ret = dwarf_readu8 (&addr, &version, arg)) < 0)
    return ret;

  if (version != 1 && version != DWARF_CIE_VERSION)
    {
      Debug (1, "Got CIE version %u, expected version 1 or ", DWARF_CIE_VERSION, "\n", version);
      return -1;
    }

  /* read and parse the augmentation string: */
  memset (augstr, 0, sizeof (augstr));
  for (i = 0;;)
    {
      if ((ret = dwarf_readu8 (&addr, &ch, arg)) < 0)
	return ret;

      if (!ch)
	break;	/* end of augmentation string */

      if (i < sizeof (augstr) - 1)
	augstr[i++] = ch;
    }

  if ((ret = dwarf_read_uleb128 (&addr, &dci->code_align, arg)) < 0
      || (ret = dwarf_read_sleb128 (&addr, &dci->data_align, arg)) < 0)
    return ret;

  /* Read the return-address column either as a u8 or as a uleb128.  */
  if (version == 1)
    {
      if ((ret = dwarf_readu8 (&addr, &ch, arg)) < 0)
	return ret;
      dci->ret_addr_column = ch;
    }
  else if ((ret = dwarf_read_uleb128 (&addr, &dci->ret_addr_column,
				      arg)) < 0)
    return ret;

  if (augstr[0] == 'z')
    {
      dci->sized_augmentation = 1;
      if ((ret = dwarf_read_uleb128 (&addr, &aug_size, arg)) < 0)
	return ret;
    }

  for (i = 1; i < sizeof (augstr) && augstr[i]; ++i)
    switch (augstr[i])
      {
      case 'L':
	/* read the LSDA pointer-encoding format.  */
	if ((ret = dwarf_readu8 (&addr, &ch, arg)) < 0)
	  return ret;
	dci->lsda_encoding = ch;
	break;

      case 'R':
	/* read the FDE pointer-encoding format.  */
	if ((ret = dwarf_readu8 (&addr, &fde_encoding, arg)) < 0)
	  return ret;
	break;

      case 'P':
	/* read the personality-routine pointer-encoding format.  */
	if ((ret = dwarf_readu8 (&addr, &handler_encoding, arg)) < 0)
	  return ret;
	if ((ret = dwarf_read_encoded_pointer (&addr, handler_encoding, pi, &dci->handler, arg)) < 0)
	  return ret;
	break;

      case 'S':
	/* Temporarily set it to one so dwarf_parse_fde() knows that
	   it should fetch the actual ABI/TAG pair from the FDE.  */
	dci->have_abi_marker = 1;
	break;

      default:
	if (dci->sized_augmentation)
	  /* If we have the size of the augmentation body, we can skip
	     over the parts that we don't understand, so we're OK. */
	  return 0;
	else
	  {
	    Debug (1, "Unexpected augmentation string `%s'\n", augstr);
	    return -1;
	  }
      }
  dci->fde_encoding = fde_encoding;
  dci->cie_instr_start = addr;
  Debug (15, "CIE parsed OK, augmentation = \"%s\", handler=0x%lx\n",
	 augstr, (long) dci->handler);
  return 0;
}

 int dwarf_extract_proc_info_from_fde ( unw_word_t *addrp, unw_proc_info_t *pi, int need_unwind_info, void *arg)
{
  unw_word_t fde_end_addr, cie_addr, cie_offset_addr, aug_end_addr = 0;
  unw_word_t start_ip, ip_range, aug_size, addr = *addrp;
  int ret, ip_range_encoding;
  struct dwarf_cie_info dci;
  uint64_t u64val;
  uint32_t u32val;

  Debug (12, "FDE @ 0x%lx\n", (long) addr);

  memset (&dci, 0, sizeof (dci));

  if ((ret = dwarf_readu32 (&addr, &u32val, arg)) < 0)
    return ret;

  if (u32val != 0xffffffff)
    {
      uint32_t cie_offset;

      /* In some configurations, an FDE with a 0 length indicates the
	 end of the FDE-table.  */
      if (u32val == 0)
	return -1;

      /* the FDE is in the 32-bit DWARF format */

      *addrp = fde_end_addr = addr + u32val;
      cie_offset_addr = addr;

      if ((ret = dwarf_readu32 (&addr, &cie_offset, arg)) < 0)
	return ret;

      if (is_cie_id (cie_offset))
	/* ignore CIEs (happens during linear searches) */
	return 0;

      /* DWARF says that the CIE_pointer in the FDE is a
	 .debug_frame-relative offset, but the GCC-generated .eh_frame
	 sections instead store a "pcrelative" offset, which is just
	 as fine as it's self-contained.  */
      cie_addr = cie_offset_addr - cie_offset;
    }
  else
    {
      uint64_t cie_offset;

      /* the FDE is in the 64-bit DWARF format */

      if ((ret = dwarf_readu64 (&addr, &u64val, arg)) < 0)
	return ret;

      *addrp = fde_end_addr = addr + u64val;
      cie_offset_addr = addr;

      if ((ret = dwarf_readu64 (&addr, &cie_offset, arg)) < 0)
	return ret;

      if (is_cie_id (cie_offset))
	/* ignore CIEs (happens during linear searches) */
	return 0;

      /* DWARF says that the CIE_pointer in the FDE is a
	 .debug_frame-relative offset, but the GCC-generated .eh_frame
	 sections instead store a "pcrelative" offset, which is just
	 as fine as it's self-contained.  */
      cie_addr = (unw_word_t) ((uint64_t) cie_offset_addr - cie_offset);
    }

  if ((ret = parse_cie (cie_addr, pi, &dci, arg)) < 0)
    return ret;

  /* IP-range has same encoding as FDE pointers, except that it's
     always an absolute value: */
  ip_range_encoding = dci.fde_encoding & DW_EH_PE_FORMAT_MASK;

  if ((ret = dwarf_read_encoded_pointer (&addr, dci.fde_encoding,
					 pi, &start_ip, arg)) < 0
      || (ret = dwarf_read_encoded_pointer (&addr, ip_range_encoding,
					    pi, &ip_range, arg)) < 0)
    return ret;
  pi->start_ip = start_ip;
  pi->end_ip = start_ip + ip_range;
  pi->handler = dci.handler;

  if (dci.sized_augmentation)
    {
      if ((ret = dwarf_read_uleb128 (&addr, &aug_size, arg)) < 0)
	return ret;
      aug_end_addr = addr + aug_size;
    }

  if ((ret = dwarf_read_encoded_pointer (&addr, dci.lsda_encoding,
					 pi, &pi->lsda, arg)) < 0)
    return ret;

  Debug (15, "FDE covers IP 0x%lx-0x%lx, LSDA=0x%lx\n",
	 (long) pi->start_ip, (long) pi->end_ip, (long) pi->lsda);

  if (need_unwind_info)
    {
      pi->unwind_info_size = sizeof (dci);
      pi->unwind_info = malloc (sizeof(struct dwarf_cie_info));
      if (!pi->unwind_info)
	return -1;

      if (dci.have_abi_marker)
	{
	  if ((ret = dwarf_readu16 (&addr, &dci.abi, arg)) < 0
	      || (ret = dwarf_readu16 (&addr, &dci.tag, arg)) < 0)
	    return ret;
	  Debug (13, "Found ABI marker = (abi=%u, tag=%u)\n",
		 dci.abi, dci.tag);
	}

      if (dci.sized_augmentation)
	dci.fde_instr_start = aug_end_addr;
      else
	dci.fde_instr_start = addr;
      dci.fde_instr_end = fde_end_addr;

      memcpy (pi->unwind_info, &dci, sizeof (dci));
    }
  return 0;
}

int read_regnum (unw_word_t *addr, unw_word_t *valp, void *arg)
{
  int ret;

  if ((ret = dwarf_read_uleb128 (addr, valp, arg)) < 0)
    return ret;

  if (*valp >= DWARF_NUM_PRESERVED_REGS)
    {
      Debug (1, "Invalid register number %u\n", (unsigned int) *valp);
      return -1;
    }
  return 0;
}

void set_reg (dwarf_state_record_t *sr, unw_word_t regnum, dwarf_where_t where, unw_word_t val)
{
  sr->rs_current.reg[regnum].where = where;
  sr->rs_current.reg[regnum].val = val;
}

int run_cfi_program (struct dwarf_cursor *c, dwarf_state_record_t *sr, unw_word_t ip, unw_word_t *addr, unw_word_t end_addr, struct dwarf_cie_info *dci)
{
  unw_word_t curr_ip, operand = 0, regnum, val, len, fde_encoding;
  dwarf_reg_state_t *rs_stack = NULL, *new_rs, *old_rs;
  uint8_t u8, op;
  uint16_t u16;
  uint32_t u32;
  void *arg;
  int ret;

  arg = c->as_arg;
  curr_ip = c->pi.start_ip;

  while (curr_ip < ip && *addr < end_addr)
    {
      if ((ret = dwarf_readu8 (addr, &op, arg)) < 0)
	return ret;

      if (op & DWARF_CFA_OPCODE_MASK)
	{
	  operand = op & DWARF_CFA_OPERAND_MASK;
	  op &= ~DWARF_CFA_OPERAND_MASK;
	}

    switch (op)
	{
	case DW_CFA_advance_loc:
	  curr_ip += operand * dci->code_align;
	  Debug (15, "CFA_advance_loc to 0x%lx\n", (long) curr_ip);
	  break;

	case DW_CFA_advance_loc1:
	  if ((ret = dwarf_readu8 (addr, &u8, arg)) < 0)
	    goto fail;
	  curr_ip += u8 * dci->code_align;
	  Debug (15, "CFA_advance_loc1 to 0x%lx\n", (long) curr_ip);
	  break;

	case DW_CFA_advance_loc2:
	  if ((ret = dwarf_readu16 (addr, &u16, arg)) < 0)
	    goto fail;
	  curr_ip += u16 * dci->code_align;
	  Debug (15, "CFA_advance_loc2 to 0x%lx\n", (long) curr_ip);
	  break;

	case DW_CFA_advance_loc4:
	  if ((ret = dwarf_readu32 (addr, &u32, arg)) < 0)
	    goto fail;
	  curr_ip += u32 * dci->code_align;
	  Debug (15, "CFA_advance_loc4 to 0x%lx\n", (long) curr_ip);
	  break;

	case DW_CFA_offset:
	  regnum = operand;
	  if (regnum >= DWARF_NUM_PRESERVED_REGS)
	    {
	      Debug (1, "Invalid register number %u in DW_cfa_OFFSET\n",
		     (unsigned int) regnum);
	      ret = -1;
	      goto fail;
	    }
	  if ((ret = dwarf_read_uleb128 (addr, &val, arg)) < 0)
	    goto fail;
	  set_reg (sr, regnum, DWARF_WHERE_CFAREL, val * dci->data_align);
	  Debug (15, "CFA_offset r%lu at cfa+0x%lx\n",
		 (long) regnum, (long) (val * dci->data_align));
	  break;

	case DW_CFA_offset_extended:
	  if (((ret = read_regnum (addr, &regnum, arg)) < 0)
	      || ((ret = dwarf_read_uleb128 (addr, &val, arg)) < 0))
	    goto fail;
	  set_reg (sr, regnum, DWARF_WHERE_CFAREL, val * dci->data_align);
	  Debug (15, "CFA_offset_extended r%lu at cf+0x%lx\n",
		 (long) regnum, (long) (val * dci->data_align));
	  break;

	case DW_CFA_offset_extended_sf:
	  if (((ret = read_regnum (addr, &regnum, arg)) < 0)
	      || ((ret = dwarf_read_sleb128 (addr, &val, arg)) < 0))
	    goto fail;
	  set_reg (sr, regnum, DWARF_WHERE_CFAREL, val * dci->data_align);
	  Debug (15, "CFA_offset_extended_sf r%lu at cf+0x%lx\n",
		 (long) regnum, (long) (val * dci->data_align));
	  break;

	case DW_CFA_restore:
	  regnum = operand;
	  if (regnum >= DWARF_NUM_PRESERVED_REGS)
	    {
	      Debug (1, "Invalid register number %u in DW_CFA_restore\n",
		     (unsigned int) regnum);
	      ret = -1;
	      goto fail;
	    }
	  sr->rs_current.reg[regnum] = sr->rs_initial.reg[regnum];
	  Debug (15, "CFA_restore r%lu\n", (long) regnum);
	  break;

	case DW_CFA_restore_extended:
	  if ((ret = dwarf_read_uleb128 (addr, &regnum, arg)) < 0)
	    goto fail;
	  if (regnum >= DWARF_NUM_PRESERVED_REGS)
	    {
	      Debug (1, "Invalid register number %u in "
		     "DW_CFA_restore_extended\n", (unsigned int) regnum);
	      ret = -1;
	      goto fail;
	    }
	  sr->rs_current.reg[regnum] = sr->rs_initial.reg[regnum];
	  Debug (15, "CFA_restore_extended r%lu\n", (long) regnum);
	  break;

	case DW_CFA_set_loc:
	  fde_encoding = dci->fde_encoding;
	  if ((ret = dwarf_read_encoded_pointer (addr, fde_encoding,
						 &c->pi, &curr_ip,
						 arg)) < 0)
	    goto fail;
	  Debug (15, "CFA_set_loc to 0x%lx\n", (long) curr_ip);
	  break;

	case DW_CFA_undefined:
	  if ((ret = read_regnum (addr, &regnum, arg)) < 0)
	    goto fail;
	  set_reg (sr, regnum, DWARF_WHERE_UNDEF, 0);
	  Debug (15, "CFA_undefined r%lu\n", (long) regnum);
	  break;

	case DW_CFA_same_value:
	  if ((ret = read_regnum (addr, &regnum, arg)) < 0)
	    goto fail;
	  set_reg (sr, regnum, DWARF_WHERE_SAME, 0);
	  Debug (15, "CFA_same_value r%lu\n", (long) regnum);
	  break;

	case DW_CFA_register:
	  if (((ret = read_regnum (addr, &regnum, arg)) < 0)
	      || ((ret = dwarf_read_uleb128 (addr, &val, arg)) < 0))
	    goto fail;
	  set_reg (sr, regnum, DWARF_WHERE_REG, val);
	  Debug (15, "CFA_register r%lu to r%lu\n", (long) regnum, (long) val);
	  break;

	case DW_CFA_remember_state:
      /*new_rs = (dwarf_reg_state_t*)(malloc (sizeof(dwarf_reg_state_t)));*/

	  /*if (!new_rs)*/
		/*{*/
		  /*Debug (1, "Out of memory in DW_CFA_remember_state\n");*/
		  /*ret = -1;*/
		  /*goto fail;*/
		/*}*/

	  /*memcpy (new_rs->reg, sr->rs_current.reg, sizeof (new_rs->reg));*/
	  /*new_rs->next = rs_stack;*/
	  /*rs_stack = new_rs;*/
	  Debug (15, "CFA_remember_state\n");
	  break;

	case DW_CFA_restore_state:
	  if (!rs_stack)
	    {
	      Debug (1, "register-state stack underflow\n");
	      ret = -1;
	      goto fail;
	    }
	  memcpy (&sr->rs_current.reg, &rs_stack->reg, sizeof (rs_stack->reg));
	  old_rs = rs_stack;
	  rs_stack = rs_stack->next;
	  free(old_rs);
	  Debug (15, "CFA_restore_state\n");
	  break;

	case DW_CFA_def_cfa:
	  if (((ret = read_regnum (addr, &regnum, arg)) < 0)
	      || ((ret = dwarf_read_uleb128 (addr, &val, arg)) < 0))
	    goto fail;
	  set_reg (sr, DWARF_CFA_REG_COLUMN, (dwarf_where_t)DWARF_WHERE_REG, regnum);
	  set_reg (sr, DWARF_CFA_OFF_COLUMN, (dwarf_where_t)0, val);	/* NOT factored! */
	  Debug (15, "CFA_def_cfa r%lu+0x%lx\n", (long) regnum, (long) val);
	  break;

	case DW_CFA_def_cfa_sf:
	  if (((ret = read_regnum (addr, &regnum, arg)) < 0)
	      || ((ret = dwarf_read_sleb128 (addr, &val, arg)) < 0))
	    goto fail;
	  set_reg (sr, DWARF_CFA_REG_COLUMN, DWARF_WHERE_REG, regnum);
	  set_reg (sr, DWARF_CFA_OFF_COLUMN, (dwarf_where_t)0, val * dci->data_align);		/* factored! */
	  Debug (15, "CFA_def_cfa_sf r%lu+0x%lx\n",
		 (long) regnum, (long) (val * dci->data_align));
	  break;

	case DW_CFA_def_cfa_register:
	  if ((ret = read_regnum (addr, &regnum, arg)) < 0)
	    goto fail;
	  set_reg (sr, DWARF_CFA_REG_COLUMN, DWARF_WHERE_REG, regnum);
	  Debug (15, "CFA_def_cfa_register r%lu\n", (long) regnum);
	  break;

	case DW_CFA_def_cfa_offset:
	  if ((ret = dwarf_read_uleb128 (addr, &val, arg)) < 0)
	    goto fail;
	  set_reg (sr, DWARF_CFA_OFF_COLUMN, (dwarf_where_t)0, val);	/* NOT factored! */
	  Debug (15, "CFA_def_cfa_offset 0x%lx\n", (long) val);
	  break;

	case DW_CFA_def_cfa_offset_sf:
	  if ((ret = dwarf_read_sleb128 (addr, &val, arg)) < 0)
	    goto fail;
	  set_reg (sr, DWARF_CFA_OFF_COLUMN, (dwarf_where_t)0, val * dci->data_align);	/* factored! */
	  Debug (15, "CFA_def_cfa_offset_sf 0x%lx\n",
		 (long) (val * dci->data_align));
	  break;

	case DW_CFA_nop:
	case DW_CFA_GNU_window_save:
	case DW_CFA_def_cfa_expression:
	case DW_CFA_expression:
	  break;

	case DW_CFA_GNU_args_size:
	  if ((ret = dwarf_read_uleb128 (addr, &val, arg)) < 0)
	    goto fail;
	  sr->args_size = val;
	  Debug (15, "CFA_GNU_args_size %lu\n", (long) val);
	  break;

	}
    }
  ret = 0;

 fail:
  /* Free the register-state stack, if not empty already.  */
  while (rs_stack)
    {
      old_rs = rs_stack;
      rs_stack = rs_stack->next;
      free(old_rs);
    }
  return ret;
}

int callback (struct dl_phdr_info *info, size_t size, void *ptr)
{
  struct callback_data *cb_data = (callback_data*)ptr;
  unw_dyn_info_t *di = &cb_data->di;
  const ElfW(Phdr) *phdr, *p_eh_hdr, *p_dynamic, *p_text;
  unw_word_t addr, eh_frame_start, eh_frame_end, fde_count, ip;
  ElfW(Addr) load_base, segbase = 0, max_load_addr = 0;
  int ret, need_unwind_info = cb_data->need_unwind_info;
  unw_proc_info_t *pi = cb_data->pi;
  struct dwarf_eh_frame_hdr *hdr;
  long n;
  int found = 0;

  ip = cb_data->ip;

  if (size < offsetof (struct dl_phdr_info, dlpi_phnum)
	     + sizeof (info->dlpi_phnum))
    return -1;

  phdr = info->dlpi_phdr;
  load_base = info->dlpi_addr;
  p_text = NULL;
  p_eh_hdr = NULL;
  p_dynamic = NULL;

  for (n = info->dlpi_phnum; --n >= 0; phdr++)
    {
      if (phdr->p_type == PT_LOAD)
	{
	  ElfW(Addr) vaddr = phdr->p_vaddr + load_base;

	  if (ip >= vaddr && ip < vaddr + phdr->p_memsz)
	    p_text = phdr;

	  if (vaddr + phdr->p_filesz > max_load_addr)
	    max_load_addr = vaddr + phdr->p_filesz;
	}
      else if (phdr->p_type == PT_GNU_EH_FRAME)
	p_eh_hdr = phdr;
      else if (phdr->p_type == PT_DYNAMIC)
	p_dynamic = phdr;
    }

  if (!p_text)
    return 0;

  if (p_eh_hdr)
    {
      if (p_eh_hdr->p_vaddr >= p_text->p_vaddr
		  && p_eh_hdr->p_vaddr < p_text->p_vaddr + p_text->p_memsz)
	segbase = p_text->p_vaddr + load_base;
      else
	{
	  phdr = info->dlpi_phdr;
	  for (n = info->dlpi_phnum; --n >= 0; phdr++)
	    {
	      if (phdr->p_type == PT_LOAD && p_eh_hdr->p_vaddr >= phdr->p_vaddr
		  && p_eh_hdr->p_vaddr < phdr->p_vaddr + phdr->p_memsz)
		{
		  segbase = phdr->p_vaddr + load_base;
		  break;
		}
	    }
	}

      if (p_dynamic)
	{
	  ElfW(Dyn) *dyn = (ElfW(Dyn) *)(p_dynamic->p_vaddr + load_base);
	  for (; dyn->d_tag != DT_NULL; ++dyn)
	    if (dyn->d_tag == DT_PLTGOT)
	      {
		/* Assume that _DYNAMIC is writable and GLIBC has
		   relocated it (true for x86 at least).  */
		di->gp = dyn->d_un.d_ptr;
		break;
	      }
	}
      else
	di->gp = 0;
      pi->gp = di->gp;

      hdr = (struct dwarf_eh_frame_hdr *) (p_eh_hdr->p_vaddr + load_base);
      if (hdr->version != DW_EH_VERSION)
	{
	  Debug (1, "table `%s' has unexpected version %d\n",
		 info->dlpi_name, hdr->version);
	  return 0;
	}

          addr = (unw_word_t) (uintptr_t) (hdr + 1);

     if ((ret = dwarf_read_encoded_pointer ( &addr, hdr->eh_frame_ptr_enc, pi, &eh_frame_start, NULL)) < 0)
	    return ret;

      if ((ret = dwarf_read_encoded_pointer ( &addr, hdr->fde_count_enc, pi, &fde_count, NULL)) < 0)
        return ret;

	  di->start_ip = p_text->p_vaddr + load_base;
	  di->end_ip = p_text->p_vaddr + load_base + p_text->p_memsz;
	  di->u.rti.name_ptr = (unw_word_t) (uintptr_t) info->dlpi_name;
	  di->u.rti.table_data = addr;
	  assert (sizeof (struct table_entry) % sizeof (unw_word_t) == 0);
	  di->u.rti.table_len = (fde_count * sizeof (struct table_entry)
				 / sizeof (unw_word_t));

	  di->u.rti.segbase = (unw_word_t) (uintptr_t) hdr;

	  found = 1;
	  Debug (15, "found table `%s': segbase=0x%lx, len=%lu, gp=0x%lx, "
		 "table_data=0x%lx\n", (char *) (uintptr_t) di->u.rti.name_ptr,
		 (long) di->u.rti.segbase, (long) di->u.rti.table_len,
		 (long) di->gp, (long) di->u.rti.table_data);
    }

  return found;
}

const struct table_entry * lookup (struct table_entry *table, size_t table_size, int32_t rel_ip)
{
  unsigned long table_len = table_size / sizeof (struct table_entry);
  const struct table_entry *e = 0;
  unsigned long lo, hi, mid;

  for (lo = 0, hi = table_len; lo < hi;)
    {
      mid = (lo + hi) / 2;
      e = table + mid;
      if (rel_ip < e->start_ip_offset)
	hi = mid;
      else
	lo = mid + 1;
    }
  if (hi <= 0)
	return NULL;
  e = table + hi - 1;
  return e;
}

int dwarf_search_unwind_table (unw_word_t ip, unw_dyn_info_t *di, unw_proc_info_t *pi, int need_unwind_info, void *arg)
{
  const struct table_entry *e = NULL, *table;
  unw_word_t segbase = 0, fde_addr;

  int ret;
  unw_word_t debug_frame_base;
  size_t table_len;

  table = (const struct table_entry *) (uintptr_t) di->u.rti.table_data;
  table_len = di->u.rti.table_len * sizeof (unw_word_t);
  debug_frame_base = 0;

  segbase = di->u.rti.segbase;
  e = lookup ((struct table_entry *) di->u.rti.table_data, di->u.rti.table_len * sizeof (unw_word_t), ip - segbase);

  if (!e)
    {
      return -1;
    }

  fde_addr = e->fde_offset + segbase;

  if ((ret = dwarf_extract_proc_info_from_fde (&fde_addr, pi, need_unwind_info, arg)) < 0)
    return ret;

  if (ip < pi->start_ip || ip >= pi->end_ip)
    return -1;

  return 0;
}

int dwarf_find_proc_info (unw_word_t ip, unw_proc_info_t *pi, int need_unwind_info, void *arg)
{
  struct callback_data cb_data;

  int ret;

  memset (&cb_data, 0, sizeof (cb_data));
  cb_data.ip = ip;
  cb_data.pi = pi;
  cb_data.need_unwind_info = need_unwind_info;

  ret = dl_iterate_phdr (callback, &cb_data);

  if(ret <= 0)
    {
      return -1;
    }

  if (cb_data.single_fde)
  {
    return 0;
  }
  else
  {
    return dwarf_search_unwind_table (ip, &cb_data.di, pi, need_unwind_info, arg);
  }

  return ret;
}

int fetch_proc_info (struct dwarf_cursor *c, unw_word_t ip, int need_unwind_info)
{
  int ret;

  --ip;

  if (!need_unwind_info)
  {
    return 0;
  }
  memset (&c->pi, 0, sizeof (c->pi));

  if(( ret = dwarf_find_proc_info((ip), &(c)->pi, (need_unwind_info), (c)->as_arg)) < 0)
  {
      return ret;
  }

  return ret;
}

void put_unwind_info (struct dwarf_cursor *c, unw_proc_info_t *pi)
{
  if (pi->unwind_info);
    {
      free (pi->unwind_info);
      pi->unwind_info = NULL;
    }
}

int parse_fde (struct dwarf_cursor *c, unw_word_t ip, dwarf_state_record_t *sr)
{
  struct dwarf_cie_info *dci;
  unw_word_t addr;
  int ret;

  dci = (dwarf_cie_info *)c->pi.unwind_info;
  c->ret_addr_column = dci->ret_addr_column;

  addr = dci->cie_instr_start;
  if ((ret = run_cfi_program (c, sr, ~(unw_word_t) 0, &addr, dci->cie_instr_end, dci)) < 0)
    return ret;

  memcpy (&sr->rs_initial, &sr->rs_current, sizeof (sr->rs_initial));

  addr = dci->fde_instr_start;

  if ((ret = run_cfi_program (c, sr, ip, &addr, dci->fde_instr_end, dci)) < 0)
    return ret;

  return 0;
}

int create_state_record_for (struct dwarf_cursor *c, dwarf_state_record_t *sr, unw_word_t ip)
{
  int i, ret;

  memset (sr, 0, sizeof (*sr));
  for (i = 0; i < DWARF_NUM_PRESERVED_REGS + 2; ++i)
  {
    set_reg (sr, i, DWARF_WHERE_SAME, 0);
  }

  ret = parse_fde (c, ip, sr);

  return ret;
}


int tdep_access_reg (struct cursor *c, unw_regnum_t reg, unw_word_t *valp, int write)
{
  dwarf_loc_t loc = DWARF_NULL_LOC;
  unsigned int mask;
  int arg_num;

  switch (reg)
    {

    case UNW_X86_EIP:
      if (write)
	c->dwarf.ip = *valp;		/* also update the EIP cache */
      loc = c->dwarf.loc[EIP];
      break;

    case UNW_X86_CFA:
    case UNW_X86_ESP:
      if (write)
	return -1;

      *valp = c->dwarf.cfa;
      return 0;

    case UNW_X86_EAX:
    case UNW_X86_EDX:
      arg_num = reg - UNW_X86_EAX;
      mask = (1 << arg_num);
      if (write)
	{
	  c->dwarf.eh_args[arg_num] = *valp;
	  c->dwarf.eh_valid_mask |= mask;
	  return 0;
	}
      else if ((c->dwarf.eh_valid_mask & mask) != 0)
	{
	  *valp = c->dwarf.eh_args[arg_num];
	  return 0;
	}
      else
	loc = c->dwarf.loc[(reg == UNW_X86_EAX) ? EAX : EDX];
      break;

    case UNW_X86_ECX: loc = c->dwarf.loc[ECX]; break;
    case UNW_X86_EBX: loc = c->dwarf.loc[EBX]; break;

    case UNW_X86_EBP: loc = c->dwarf.loc[EBP]; break;
    case UNW_X86_ESI: loc = c->dwarf.loc[ESI]; break;
    case UNW_X86_EDI: loc = c->dwarf.loc[EDI]; break;


    default:
      Debug (1, "bad register number %u\n", reg);
      return -1;
    }

  if (write)
    return dwarf_put (&c->dwarf, loc, *valp);
  else
    return dwarf_get (&c->dwarf, loc, valp);
}


void *tdep_uc_addr (ucontext_t *uc, int reg)
{
  void *addr;

  switch (reg)
    {
    case UNW_X86_EAX: addr = &uc->uc_mcontext.gregs[REG_EAX]; break;
    case UNW_X86_EBX: addr = &uc->uc_mcontext.gregs[REG_EBX]; break;
    case UNW_X86_ECX: addr = &uc->uc_mcontext.gregs[REG_ECX]; break;
    case UNW_X86_EDX: addr = &uc->uc_mcontext.gregs[REG_EDX]; break;
    case UNW_X86_ESI: addr = &uc->uc_mcontext.gregs[REG_ESI]; break;
    case UNW_X86_EDI: addr = &uc->uc_mcontext.gregs[REG_EDI]; break;
    case UNW_X86_EBP: addr = &uc->uc_mcontext.gregs[REG_EBP]; break;
    case UNW_X86_EIP: addr = &uc->uc_mcontext.gregs[REG_EIP]; break;
    case UNW_X86_ESP: addr = &uc->uc_mcontext.gregs[REG_ESP]; break;

    default:
      addr = NULL;
    }
  return addr;
}

unw_word_t unw_get_ip(unw_cursor_t *c)
{
  return ((cursor*)c)->dwarf.ip;
}

int unw_get_reg (unw_cursor_t *cursor, int regnum, unw_word_t *valp)
{
  struct cursor *c = (struct cursor *) cursor;

  if (regnum == UNW_REG_IP)
    {
      *valp = unw_get_ip((unw_cursor_t*)c);
      return 0;
    }

  return tdep_access_reg (c, regnum, valp, 0);
}

int apply_reg_state (struct dwarf_cursor *c, struct dwarf_reg_state *rs)
{
  unw_word_t regnum, addr, cfa, ip;
  unw_word_t prev_ip, prev_cfa;
  dwarf_loc_t cfa_loc;
  int i, ret;
  void *arg;

  prev_ip = c->ip;
  prev_cfa = c->cfa;

  arg = c->as_arg;

  if (rs->reg[DWARF_CFA_REG_COLUMN].where == DWARF_WHERE_REG)
    {

      if ((rs->reg[DWARF_CFA_REG_COLUMN].val == UNW_TDEP_SP)
	  && (rs->reg[UNW_TDEP_SP].where == DWARF_WHERE_SAME))
	  cfa = c->cfa;
      else
	{
	  regnum = dwarf_to_unw_regnum (rs->reg[DWARF_CFA_REG_COLUMN].val);
	  if ((ret = unw_get_reg ((unw_cursor_t *) c, regnum, &cfa)) < 0)
	    return ret;
	}
      cfa += rs->reg[DWARF_CFA_OFF_COLUMN].val;
    }
  else
    {
      assert (rs->reg[DWARF_CFA_REG_COLUMN].where == DWARF_WHERE_EXPR);

      addr = rs->reg[DWARF_CFA_REG_COLUMN].val;
      /*if ((ret = eval_location_expr (c, as, a, addr, &cfa_loc, arg)) < 0)*/
	return ret;
      /* the returned location better be a memory location... */
      if (DWARF_IS_REG_LOC (cfa_loc))
	return -1;
      cfa = cfa_loc.val;
    }

  for (i = 0; i < DWARF_NUM_PRESERVED_REGS; ++i)
    {
      switch ((dwarf_where_t) rs->reg[i].where)
	{
	case DWARF_WHERE_UNDEF:
	  c->loc[i] = DWARF_NULL_LOC;
	  break;

	case DWARF_WHERE_SAME:
	  break;

	case DWARF_WHERE_CFAREL:
	  c->loc[i] = DWARF_MEM_LOC (c, cfa + rs->reg[i].val);
	  break;

	case DWARF_WHERE_REG:
	  c->loc[i] = DWARF_REG_LOC (c, dwarf_to_unw_regnum (rs->reg[i].val));
	  break;

	case DWARF_WHERE_EXPR:
	  addr = rs->reg[i].val;
	  /*if ((ret = eval_location_expr (c, as, a, addr, c->loc + i, arg)) , 0)*/
	    return ret;
	  break;
	}
    }
  c->cfa = cfa;
  ret = dwarf_get (c, c->loc[c->ret_addr_column], &ip);
  if (ret < 0)
    return ret;
  c->ip = ip;
  if (c->ip == prev_ip && c->cfa == prev_cfa)
    {
      return -1;
    }
  return 0;
}


int common_init (struct cursor *c)
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

  c->dwarf.ip = *((unw_word_t *)c->dwarf.loc[EIP].val);

  ret = dwarf_get (&c->dwarf, DWARF_REG_LOC (&c->dwarf, UNW_X86_ESP), &c->dwarf.cfa);

  c->dwarf.ret_addr_column = 0;

  return 0;
}

int unw_init_local(unw_cursor_t *cursor, ucontext_t *uc)
{
  struct cursor *c = (struct cursor *) cursor;

  c->dwarf.as_arg = c;
  c->uc = uc;

  return common_init(c);
}

int unw_step(unw_cursor_t *cursor)
{
    struct cursor *c = (struct cursor *) cursor;

    int ret;
    dwarf_state_record_t sr;

    if ((ret = fetch_proc_info (((dwarf_cursor*)(&c->dwarf)), ((dwarf_cursor*)(&c->dwarf))->ip, 1)) < 0)
    {
        return ret;
    }

    if ((ret = create_state_record_for (((dwarf_cursor*)(&c->dwarf)), &sr, ((dwarf_cursor*)(&c->dwarf))->ip)) < 0)
    {
        return ret;
    }

    if ((ret = apply_reg_state (((dwarf_cursor*)(&c->dwarf)), &sr.rs_current)) < 0)
    {
        return ret;
    }

    //clear after unwind
    put_unwind_info (((dwarf_cursor*)(&c->dwarf)), &((dwarf_cursor*)(&c->dwarf))->pi);

    return 0;
}

int access_reg (unw_regnum_t reg, unw_word_t *val, int write, void *arg)
{
  unw_word_t *addr;

  ucontext_t *uc = ((struct cursor *)arg)->uc;

  if (!(addr = (unw_word_t*)tdep_uc_addr (uc, reg)))
  {
    goto badreg;
  }

  if (write)
    {
      *(unw_word_t *)addr = *val;
    }
  else
    {
      *val = *(unw_word_t *) addr;
    }

  return 0;

 badreg:
  return -1;
}

int establish_machine_state (struct cursor *c)
{
  void *arg = c->dwarf.as_arg;
  unw_word_t val;
  int reg;

  for (reg = 0; reg <= UNW_REG_LAST; ++reg)
    {
        if (tdep_access_reg (c, reg, &val, 0) >= 0)
        {
         access_reg(reg, &val, 1, arg);
        }
    }

  return 0;
}

int x86_local_resume (unw_cursor_t *cursor, void *arg)
{
  struct cursor *c = (struct cursor *) cursor;
  ucontext_t *uc = c->uc;

  setcontext (uc);

  return 0;
}

int unw_resume (unw_cursor_t *p_cursor)
{
  int ret;

  if ((ret = establish_machine_state ((cursor*)p_cursor)) < 0)
    return ret;

    return  x86_local_resume((unw_cursor_t *) p_cursor, ((cursor*)p_cursor)->dwarf.as_arg);
}

