INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_ClearRect
PUBLIC SP1CRSELECT
PUBLIC l_jpix

EXTERN asm_sp1_GetUpdateStruct

asm_sp1_ClearRect:

   and $07
   ret z

   push hl
   call SP1CRSELECT
   call asm_sp1_GetUpdateStruct
   pop de

.ClearRect_rowloop

   push bc
   push hl

.ClearRect_colloop

   call l_jpix
   djnz ClearRect_colloop

   pop hl
   ld bc,10*SP1V_DISPWIDTH
   add hl,bc
   pop bc

   dec c
   jp nz, ClearRect_rowloop

   ret

SP1CRSELECT:

   push de            ; save D=row, E=col
   add a,a
   ld e,a
   ld d,0
   ld hl,ClearRect_seltbl
   add hl,de
   ld a,(hl)
   ld ixl,a
   inc hl
   ld a,(hl)
   ld ixh,a
   pop de             ; restore D=row, E=col

   ret

.ClearRect_seltbl

   defw ClearRect_OPTION0, ClearRect_OPTION1, ClearRect_OPTION2, ClearRect_OPTION3
   defw ClearRect_OPTION4, ClearRect_OPTION5, ClearRect_OPTION6, ClearRect_OPTION7

.ClearRect_OPTION0:

   ld a,10
   add a,l
   ld l,a
   ret nc
   inc h
   ret

.ClearRect_OPTION1:

   inc hl
   inc hl
   ld (hl),e
   inc hl
   ld (hl),0
   ld a,7
   add a,l
   ld l,a
   ret nc
   inc h
   ret

.ClearRect_OPTION2:

   inc hl
   ld (hl),d
   ld a,9
   add a,l
   ld l,a
   ret nc
   inc h
   ret

.ClearRect_OPTION3:

   inc hl
   ld (hl),d
   inc hl
   ld (hl),e
   inc hl
   ld (hl),0
   ld a,7
   add a,l
   ld l,a
   ret nc
   inc h
   ret

.ClearRect_OPTION4:

   ld a,(hl)
   and $c0
   inc a
   ld (hl),a
   inc hl
   inc hl
   inc hl
   inc hl
   push hl

   ld a,(hl)
   or a
   jr z, ClearRect_done

   ld (hl),0
   inc hl
   ld l,(hl)
   ld h,a

.ClearRect_loop

   dec hl
   dec hl
   dec hl
   dec hl

   ld (hl),0
   ld a,18
   add a,l
   ld l,a
   jp nc, ClearRect_noinc1
   inc h

.ClearRect_noinc1

   ld a,(hl)
   or a
   jr z, ClearRect_done

   inc hl
   ld l,(hl)
   ld h,a
   jp ClearRect_loop

.ClearRect_done

   pop hl
   ld a,6
   add a,l
   ld l,a
   ret nc
   inc h
   ret

.ClearRect_OPTION5:

   inc hl
   inc hl
   ld (hl),e
   inc hl
   ld (hl),0
   dec hl
   dec hl
   dec hl
   jp ClearRect_OPTION4

.ClearRect_OPTION6:

   inc hl
   ld (hl),d
   dec hl
   jp ClearRect_OPTION4

.ClearRect_OPTION7:

   inc hl
   ld (hl),d
   inc hl
   ld (hl),e
   inc hl
   ld (hl),0
   dec hl
   dec hl
   dec hl
   jp ClearRect_OPTION4

l_jpix:
   jp (ix)
