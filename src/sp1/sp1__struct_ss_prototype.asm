INCLUDE "sp1___config.asm"

PUBLIC _sp1_struct_ss_prototype

_sp1_struct_ss_prototype:

   defb 0
   defb SP1V_DISPWIDTH
   defb 1
   defb 0
   defb 0
   defb 0
   defw 0

   ld a,SP1V_ROTTBL/256
   ld bc,0
   ex de,hl
   jp (hl)

   defw 0

   defb 1
   defb 1
   defb 0
