INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_MoveSprPix

EXTERN asm_sp1_MoveSprAbs

asm_sp1_MoveSprPix:

   ld a,e
   and $07
   srl d
   rr e
   srl d
   rr e
   srl d
   rr e
   ld d,b
   ld b,a

   ld a,c
   and $07
   srl d
   rr c
   srl d
   rr c
   srl d
   rr c
   ld d,c
   ld c,a

   jp asm_sp1_MoveSprAbs
