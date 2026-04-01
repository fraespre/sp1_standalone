INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_CreateSpr

EXTERN _sp1_struct_ss_prototype, _sp1_struct_cs_prototype
EXTERN asm_malloc, asm_free

asm_sp1_CreateSpr:

   push ix
   push iy

   push af
   ex af,af
   pop af
   exx

   ld hl,0
   push hl
   ld b,a

.CreateSpr_csalloc:

   push bc
   ld hl,24
   push hl
   call asm_malloc
   pop bc
   jp c, CreateSpr_fail

   pop bc
   push hl
   djnz CreateSpr_csalloc

   ld hl,20
   push hl
   call asm_malloc
   pop bc
   jp c, CreateSpr_fail

   push hl

   exx
   ex (sp),hl
   push de
   push bc

   ld de,_sp1_struct_ss_prototype
   ex de,hl
   push de
   pop ix
   ld bc,20
   ldir

   ex af,af
   ld (ix+3),a

   pop bc
   bit 6,b
   jr z, CreateSpr_onebyte
   set 7,(ix+4)

.CreateSpr_onebyte:

   ld a,b
   and $90
   or $40

   pop de
   pop hl
   ex (sp),hl
   push de

   ld (ix+15),h
   ld (ix+16),l

   ld de,_sp1_struct_cs_prototype
   ex de,hl
   push de
   pop iy
   push bc
   ld bc,24
   ldir
   pop bc

   ld (iy+4),c
   ld (iy+5),a

   push iy
   pop de
   ld hl,10
   add hl,de
   ex de,hl

   pop bc
   ld hl,-10
   add hl,bc
   ld bc,10
   ldir

   ld a,ixl
   add a,8
   ld (iy+8),a
   ld a,ixh
   adc a,0
   ld (iy+9),a

   pop hl
   ld (iy+11),l
   ld (iy+12),h

.CreateSpr_loop:

   pop hl

   ld a,h
   or l
   jr z, CreateSpr_done

   push hl

   ld (iy+0),h
   ld (iy+1),l

   push iy
   pop de
   ex de,hl
   ld bc,24
   ldir

   ld e,(iy+11)
   ld d,(iy+12)

   pop iy

   ld (iy+0),0
   ld (iy+1),0

   ld hl,8
   bit 7,(ix+4)
   jr z, CreateSpr_onebyte2
   ld l,16

.CreateSpr_onebyte2:

   add hl,de
   ld (iy+11),l
   ld (iy+12),h

   jp CreateSpr_loop

.CreateSpr_done:

   set 5,(iy+5)
   ld a,ixl
   ld l,a
   ld a,ixh
   ld h,a
   scf
   jr CreateSpr_all_return


.CreateSpr_fail:

   pop bc

.CreateSpr_faillp:

   pop hl

   ld a,h
   or l
   jr z, CreateSpr_all_return

   push hl
   call asm_free

   pop hl
   jp CreateSpr_faillp

.CreateSpr_all_return:
   pop iy
   pop ix
   ret
