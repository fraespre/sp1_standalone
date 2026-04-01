INCLUDE "sp1___config.asm"

PUBLIC _SP1_DRAW_MASK2RB

EXTERN _SP1_DRAW_MASK2LB
EXTERN SP1RETSPRDRAW

   ld de,0
   nop
   ld hl,0
   call _SP1_DRAW_MASK2RB

_SP1_DRAW_MASK2RB:

   cp SP1V_ROTTBL/256
   jp z, SP1RETSPRDRAW

   add hl,bc
   ld d,a
   inc d

_SP1Mask2RBRotate:

   jp _SP1_DRAW_MASK2LB + 7
