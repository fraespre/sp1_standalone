INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_ClearRectInv

EXTERN asm_sp1_GetUpdateStruct, asm_sp1_ClearRect, SP1CRSELECT, l_jpix

asm_sp1_ClearRectInv:

   and $07
   ret z

   push hl
   call SP1CRSELECT
   call asm_sp1_GetUpdateStruct
   pop de

   ld iy,(SP1V_UPDATELISTT)

.rowloop

   push bc
   push hl

.colloop

   ld a,$80
   xor (hl)
   jp p, alreadyinv
   ld (hl),a

   ld (iy+6),h
   ld (iy+7),l

   ld a,l
   ld iyl,a
   ld a,h
   ld iyh,a

.alreadyinv

   call l_jpix
   djnz colloop

   pop hl
   ld bc,10*SP1V_DISPWIDTH
   add hl,bc
   pop bc

   dec c
   jp nz, rowloop

   ld (iy+6),0
   ld (SP1V_UPDATELISTT),iy
   ret
