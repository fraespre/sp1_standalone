INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_AddColSpr

EXTERN _sp1_struct_cs_prototype
EXTERN asm_malloc, asm_free

asm_sp1_AddColSpr:

   exx
   ld hl,0
   push hl
   ld b,(ix+3)

.AddColSpr_csalloc:

   push bc
   ld hl,24
   push hl
   call asm_malloc
   pop bc
   jp c, AddColSpr_fail

   pop bc
   push hl
   djnz AddColSpr_csalloc

   exx
   ex (sp),hl
   push de

   ld de,_sp1_struct_cs_prototype
   ex de,hl
   push de
   pop iy
   push bc
   ld bc,24
   ldir
   pop bc

   pop de
   pop hl
   push bc
   ld c,e
   ld b,d

   ld (iy+4),h                ; store plane (h = plane for AddColSpr)
   ld a,l
   and $90
   or $40
   ld (iy+5),a                ; store type

   push iy
   pop de
   ld hl,10
   add hl,de
   ex de,hl
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

   pop bc
   ld (iy+11),c
   ld (iy+12),b

   ld h,(ix+15)
   ld l,(ix+16)

.AddColSpr_loop:

   ld bc,4

.AddColSpr_search:

   ld d,(hl)
   inc hl
   ld e,(hl)
   add hl,bc
   bit 6,(hl)
   ex de,hl
   jr z, AddColSpr_search

   ex de,hl
   res 6,(hl)
   ld bc,-5
   add hl,bc

   ld a,iyh
   ld (hl),a
   inc hl
   ld a,iyl
   ld (hl),a

   ld (iy+0),d
   ld (iy+1),e

   ld bc,10
   add hl,bc
   ld a,(hl)
   ld (iy+15),a
   inc hl
   ld a,(hl)
   ld (iy+16),a

   pop hl

   ld a,h
   or l
   jr z, AddColSpr_done

   push de
   push hl
   push iy
   pop de
   ex de,hl
   ld bc,24
   ldir

   ld e,(iy+11)
   ld d,(iy+12)

   pop iy

   ld hl,8
   bit 7,(ix+4)
   jr z, AddColSpr_onebyte2
   ld l,16

.AddColSpr_onebyte2:

   add hl,de
   ld (iy+11),l
   ld (iy+12),h

   pop hl
   jr AddColSpr_loop

.AddColSpr_done:

   set 5,(iy+5)
   inc (ix+2)
   inc l
   scf
   ret

.AddColSpr_fail:

   pop bc

.AddColSpr_faillp:

   pop hl

   ld a,h
   or l
   ret z

   push hl
   call asm_free

   pop hl
   jr AddColSpr_faillp
