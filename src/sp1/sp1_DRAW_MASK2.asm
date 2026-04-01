INCLUDE "sp1___config.asm"

PUBLIC _SP1_DRAW_MASK2

EXTERN _SP1_DRAW_MASK2NR
EXTERN SP1RETSPRDRAW

   ld hl,0
   ld ix,0
   call _SP1_DRAW_MASK2

_SP1_DRAW_MASK2:

   cp SP1V_ROTTBL/256
   jp z, _SP1_DRAW_MASK2NR

   add hl,bc
   add ix,bc
   ex de,hl
   ld h,a

_SP1Mask2Rotate:

   ld a,(de)
   inc de
   ld l,a
   ld a,(hl)
   inc h
   ld l,(ix+0)
   or (hl)
   ld b,a
   ld l,(ix+1)
   ld c,(hl)
   dec h
   ld a,(de)
   inc de
   ld l,a
   ld a,(SP1V_PIXELBUFFER+0)
   and b
   or c
   or (hl)
   ld (SP1V_PIXELBUFFER+0),a

   ld a,(de)
   inc de
   ld l,a
   ld a,(hl)
   inc h
   ld l,(ix+2)
   or (hl)
   ld b,a
   ld l,(ix+3)
   ld c,(hl)
   dec h
   ld a,(de)
   inc de
   ld l,a
   ld a,(SP1V_PIXELBUFFER+1)
   and b
   or c
   or (hl)
   ld (SP1V_PIXELBUFFER+1),a

   ld a,(de)
   inc de
   ld l,a
   ld a,(hl)
   inc h
   ld l,(ix+4)
   or (hl)
   ld b,a
   ld l,(ix+5)
   ld c,(hl)
   dec h
   ld a,(de)
   inc de
   ld l,a
   ld a,(SP1V_PIXELBUFFER+2)
   and b
   or c
   or (hl)
   ld (SP1V_PIXELBUFFER+2),a

   ld a,(de)
   inc de
   ld l,a
   ld a,(hl)
   inc h
   ld l,(ix+6)
   or (hl)
   ld b,a
   ld l,(ix+7)
   ld c,(hl)
   dec h
   ld a,(de)
   inc de
   ld l,a
   ld a,(SP1V_PIXELBUFFER+3)
   and b
   or c
   or (hl)
   ld (SP1V_PIXELBUFFER+3),a

   ld a,(de)
   inc de
   ld l,a
   ld a,(hl)
   inc h
   ld l,(ix+8)
   or (hl)
   ld b,a
   ld l,(ix+9)
   ld c,(hl)
   dec h
   ld a,(de)
   inc de
   ld l,a
   ld a,(SP1V_PIXELBUFFER+4)
   and b
   or c
   or (hl)
   ld (SP1V_PIXELBUFFER+4),a

   ld a,(de)
   inc de
   ld l,a
   ld a,(hl)
   inc h
   ld l,(ix+10)
   or (hl)
   ld b,a
   ld l,(ix+11)
   ld c,(hl)
   dec h
   ld a,(de)
   inc de
   ld l,a
   ld a,(SP1V_PIXELBUFFER+5)
   and b
   or c
   or (hl)
   ld (SP1V_PIXELBUFFER+5),a

   ld a,(de)
   inc de
   ld l,a
   ld a,(hl)
   inc h
   ld l,(ix+12)
   or (hl)
   ld b,a
   ld l,(ix+13)
   ld c,(hl)
   dec h
   ld a,(de)
   inc de
   ld l,a
   ld a,(SP1V_PIXELBUFFER+6)
   and b
   or c
   or (hl)
   ld (SP1V_PIXELBUFFER+6),a

   ld a,(de)
   inc de
   ld l,a
   ld a,(hl)
   inc h
   ld l,(ix+14)
   or (hl)
   ld b,a
   ld l,(ix+15)
   ld c,(hl)
   dec h
   ld a,(de)
   ld l,a
   ld a,(SP1V_PIXELBUFFER+7)
   and b
   or c
   or (hl)
   ld (SP1V_PIXELBUFFER+7),a

   jp SP1RETSPRDRAW
