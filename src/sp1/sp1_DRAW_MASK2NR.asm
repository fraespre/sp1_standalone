INCLUDE "sp1___config.asm"

PUBLIC _SP1_DRAW_MASK2NR

EXTERN SP1RETSPRDRAW

   ld hl,0
   nop
   ld de,0
   call _SP1_DRAW_MASK2NR

_SP1_DRAW_MASK2NR:

   add hl,bc

   ld de,(SP1V_PIXELBUFFER+0)
   ld a,(hl)
   and e
   inc hl
   or (hl)
   inc hl
   ld (SP1V_PIXELBUFFER+0),a
   ld a,(hl)
   and d
   inc hl
   or (hl)
   inc hl
   ld (SP1V_PIXELBUFFER+1),a

   ld de,(SP1V_PIXELBUFFER+2)
   ld a,(hl)
   and e
   inc hl
   or (hl)
   inc hl
   ld (SP1V_PIXELBUFFER+2),a
   ld a,(hl)
   and d
   inc hl
   or (hl)
   inc hl
   ld (SP1V_PIXELBUFFER+3),a

   ld de,(SP1V_PIXELBUFFER+4)
   ld a,(hl)
   and e
   inc hl
   or (hl)
   inc hl
   ld (SP1V_PIXELBUFFER+4),a
   ld a,(hl)
   and d
   inc hl
   or (hl)
   inc hl
   ld (SP1V_PIXELBUFFER+5),a

   ld de,(SP1V_PIXELBUFFER+6)
   ld a,(hl)
   and e
   inc hl
   or (hl)
   inc hl
   ld (SP1V_PIXELBUFFER+6),a
   ld a,(hl)
   and d
   inc hl
   or (hl)
   ld (SP1V_PIXELBUFFER+7),a

   jp SP1RETSPRDRAW
