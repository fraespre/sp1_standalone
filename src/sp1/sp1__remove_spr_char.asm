INCLUDE "sp1___config.asm"

PUBLIC __sp1_remove_spr_char

__sp1_remove_spr_char:

   ld de,14
   add hl,de

   ld b,(hl)
   inc b
   inc hl
   djnz _remove_spr_char_nextexists

   inc hl
   ld d,(hl)
   inc hl
   ld e,(hl)
   ex de,hl
   ld (hl),0

   ret

._remove_spr_char_nextexists:

   ld c,(hl)
   inc hl
   ld d,(hl)
   inc hl
   ld e,(hl)

   ex de,hl
   push hl

   ld (hl),b
   inc hl
   ld (hl),c

   ld hl,16
   add hl,bc
   pop bc

   ld (hl),b
   inc hl
   ld (hl),c

   ret
