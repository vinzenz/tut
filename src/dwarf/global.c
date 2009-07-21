#include "dwarf_i.h"

HIDDEN struct mempool dwarf_reg_state_pool;
HIDDEN struct mempool dwarf_cie_info_pool;

HIDDEN int
dwarf_init (void)
{
  mempool_init (&dwarf_reg_state_pool, sizeof (dwarf_reg_state_t), 0);
  mempool_init (&dwarf_cie_info_pool, sizeof (struct dwarf_cie_info), 0);
  return 0;
}
