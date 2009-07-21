#ifndef unwind_i_h
#define unwind_i_h

#include <memory.h>
#include <stdint.h>

#include <libunwind-x86.h>

#include "libunwind_i.h"

/* DWARF column numbers: */
#define EAX	0
#define ECX	1
#define EDX	2
#define EBX	3
#define ESP	4
#define EBP	5
#define ESI	6
#define EDI	7
#define EIP	8
#define EFLAGS	9
#define TRAPNO	10
#define ST0	11

#define x86_lock			UNW_OBJ(lock)
#define x86_local_resume		UNW_OBJ(local_resume)
#define x86_local_addr_space_init	UNW_OBJ(local_addr_space_init)
#define x86_scratch_loc			UNW_OBJ(scratch_loc)

extern void x86_local_addr_space_init (void);
extern int x86_local_resume (unw_addr_space_t as, unw_cursor_t *cursor,
			     void *arg);
extern dwarf_loc_t x86_scratch_loc (struct cursor *c, unw_regnum_t reg);

#endif /* unwind_i_h */
