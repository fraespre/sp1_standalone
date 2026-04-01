INCLUDE "sp1___config.asm"

PUBLIC __sp1_add_spr_char

__sp1_add_spr_char:

   ld d,(hl)
   inc hl
   inc d
   dec d
   jr z, _add_spr_char_donesearch1

   ld e,(hl)
   dec de
   dec de
   ex de,hl
   cp (hl)
   jr nc, _add_spr_char_donesearch0
   ld de,16
   add hl,de

   jp __sp1_add_spr_char

._add_spr_char_donesearch1:

   ld (hl),c
   dec hl
   ld (hl),b
   ex de,hl
   ld hl,14
   add hl,bc
   ld (hl),0
   inc hl
   inc hl
   ld (hl),d
   inc hl
   ld (hl),e

   ret

._add_spr_char_donesearch0:

   inc hl
   inc hl
   ex de,hl
   ld (hl),c
   dec hl
   ld (hl),b
   push hl
   ld hl,14
   add hl,bc
   ld (hl),d
   inc hl
   ld (hl),e
   inc hl
   pop bc
   ld (hl),b
   inc hl
   ld (hl),c
   dec hl
   dec hl
   dec hl
   ex de,hl
   ld bc,16
   add hl,bc
   ld (hl),d
   inc hl
   ld (hl),e

   ret
