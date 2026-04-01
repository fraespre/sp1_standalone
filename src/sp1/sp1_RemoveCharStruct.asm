INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_RemoveCharStruct

EXTERN __sp1_remove_spr_char

asm_sp1_RemoveCharStruct:

   inc hl
   inc hl

   ld a,(hl)
   or a
   ret z                       ; not in any struct update draw list

   ld d,a
   ld (hl),0                   ; not part of this draw list anymore
   inc hl
   ld e,(hl)                   ; de = struct update *
   inc hl
   inc hl                      ; hl = & sp1_cs.type

   bit 7,(hl)
   ex de,hl
   jr z, RemoveCharStruct_notoccluding
   dec (hl)                    ; reduce occluding count in struct update

.RemoveCharStruct_notoccluding:

   inc de                      ; de = & sp1_cs.attr_mask
   ld hl,17
   add hl,de                   ; hl = & sp1_cs.prev_in_upd + 1b
   ex de,hl
   jp __sp1_remove_spr_char
