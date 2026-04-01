INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_UpdateNow

EXTERN SP1DrawUpdateStruct

asm_sp1_UpdateNow:

   push ix
   push iy

   ld hl,(SP1V_UPDATELISTH+6)
   ld a,l
   ld l,h
   ld h,a
   or a
   jp nz, UpdateNow_updatelp

   jr UpdateNow_all_return

.UpdateNow_skipthischar

   ld bc,6
   add hl,bc
   ld a,(hl)
   or a
   jr z, UpdateNow_doneupdate
   inc hl
   ld l,(hl)
   ld h,a

.UpdateNow_updatelp

   bit 6,(hl)
   jr nz, UpdateNow_skipthischar

   ld a,$80
   xor (hl)
   jp m, UpdateNow_skipthischar
   ld (hl),a

   ld b,a

   call SP1DrawUpdateStruct

   ld l,c
   ld h,b

   inc b
   djnz UpdateNow_updatelp

.UpdateNow_doneupdate

   xor a
   ld (SP1V_UPDATELISTH+6),a
   ld hl,SP1V_UPDATELISTH
   ld (SP1V_UPDATELISTT),hl

.UpdateNow_all_return:
   pop iy
   pop ix
   ret
