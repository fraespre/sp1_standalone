INCLUDE "sp1___config.asm"

PUBLIC _sp1_struct_cs_prototype

_sp1_struct_cs_prototype:

   defw 0
   defw 0
   defb 0
   defb 0
   defb $ff
   defb 0
   defw 0

   ld hl,0
   ld ix,0
   call 0

   defw 0
   defw 0
