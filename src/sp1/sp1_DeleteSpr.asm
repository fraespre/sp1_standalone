INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_DeleteSpr

EXTERN asm_free

asm_sp1_DeleteSpr:

   ex de,hl
   ld hl,15
   add hl,de

.DeleteSpr_loop:

   ld b,(hl)
   inc hl
   ld c,(hl)
   push bc
   ex de,hl
   push hl
   call asm_free

   pop hl
   pop de
   ld l,e
   ld h,d

   inc h
   dec h
   jp nz, DeleteSpr_loop

   ret
