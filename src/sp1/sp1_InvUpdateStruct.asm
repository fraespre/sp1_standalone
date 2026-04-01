INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_InvUpdateStruct

asm_sp1_InvUpdateStruct:

   ld a,$80
   xor (hl)
   ret p
   ld (hl),a

   ex de,hl
   ld hl,(SP1V_UPDATELISTT)
   ld bc,6
   add hl,bc
   ld (hl),d
   inc hl
   ld (hl),e
   ex de,hl
   ld (SP1V_UPDATELISTT),hl
   add hl,bc
   ld (hl),0

   ret
