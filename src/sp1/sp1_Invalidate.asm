INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_Invalidate

EXTERN asm_sp1_GetUpdateStruct

asm_sp1_Invalidate:

   call asm_sp1_GetUpdateStruct
   ex de,hl
   ld hl,(SP1V_UPDATELISTT)
   ld a,6
   add a,l
   ld l,a
   jp nc, rowlp
   inc h

.rowlp

   push bc
   push de

.collp

   ld a,(de)
   xor $80
   jp p, alreadyinlist

   ld (de),a
   ld (hl),d
   inc hl
   ld (hl),e

   ld hl,6
   add hl,de

.alreadyinlist

   ld a,10
   add a,e
   ld e,a
   jp nc, noinc
   inc d

.noinc

   djnz collp

   pop de
   ex de,hl
   ld bc,10*SP1V_DISPWIDTH
   add hl,bc
   ex de,hl
   pop bc

   dec c
   jp nz, rowlp

   ld (hl),0
   ld bc,-6
   add hl,bc
   ld (SP1V_UPDATELISTT),hl

   ret
