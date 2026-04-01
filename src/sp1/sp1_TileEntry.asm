INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_TileEntry

asm_sp1_TileEntry:

   ld hl,SP1V_TILEARRAY
   ld b,0
   add hl,bc
   ld a,(hl)
   ld (hl),e
   ld e,a
   inc h
   ld a,(hl)
   ld (hl),d
   ld h,a
   ld l,e

   ret
