INCLUDE "sp1___config.asm"

PUBLIC _SP1_DRAW_MASK2LB

EXTERN _SP1_DRAW_MASK2NR
EXTERN SP1RETSPRDRAW

   ld hl,0
   nop
   ld de,0
   call _SP1_DRAW_MASK2LB

_SP1_DRAW_MASK2LB:

   cp SP1V_ROTTBL/256
   jp z, _SP1_DRAW_MASK2NR

   add hl,bc
   ld d,a

   ld e,$ff
   ld a,(de)
   cpl
   exx
   ld b,a
   exx

_SP1Mask2LBRotate:

   ld bc,(SP1V_PIXELBUFFER+0)
   ld e,(hl)
   inc hl
   ld a,(de)
   exx
   or b
   exx
   and c
   ld c,a
   ld e,(hl)
   inc hl
   ld a,(de)
   or c
   ld (SP1V_PIXELBUFFER+0),a
   ld e,(hl)
   inc hl
   ld a,(de)
   exx
   or b
   exx
   and b
   ld b,a
   ld e,(hl)
   inc hl
   ld a,(de)
   or b
   ld (SP1V_PIXELBUFFER+1),a

   ld bc,(SP1V_PIXELBUFFER+2)
   ld e,(hl)
   inc hl
   ld a,(de)
   exx
   or b
   exx
   and c
   ld c,a
   ld e,(hl)
   inc hl
   ld a,(de)
   or c
   ld (SP1V_PIXELBUFFER+2),a
   ld e,(hl)
   inc hl
   ld a,(de)
   exx
   or b
   exx
   and b
   ld b,a
   ld e,(hl)
   inc hl
   ld a,(de)
   or b
   ld (SP1V_PIXELBUFFER+3),a

   ld bc,(SP1V_PIXELBUFFER+4)
   ld e,(hl)
   inc hl
   ld a,(de)
   exx
   or b
   exx
   and c
   ld c,a
   ld e,(hl)
   inc hl
   ld a,(de)
   or c
   ld (SP1V_PIXELBUFFER+4),a
   ld e,(hl)
   inc hl
   ld a,(de)
   exx
   or b
   exx
   and b
   ld b,a
   ld e,(hl)
   inc hl
   ld a,(de)
   or b
   ld (SP1V_PIXELBUFFER+5),a

   ld bc,(SP1V_PIXELBUFFER+6)
   ld e,(hl)
   inc hl
   ld a,(de)
   exx
   or b
   exx
   and c
   ld c,a
   ld e,(hl)
   inc hl
   ld a,(de)
   or c
   ld (SP1V_PIXELBUFFER+6),a
   ld e,(hl)
   inc hl
   ld a,(de)
   exx
   or b
   exx
   and b
   ld b,a
   ld e,(hl)
   ld a,(de)
   or b
   ld (SP1V_PIXELBUFFER+7),a

   jp SP1RETSPRDRAW
