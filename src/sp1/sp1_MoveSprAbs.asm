INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_MoveSprAbs

EXTERN asm_sp1_GetUpdateStruct, __sp1_add_spr_char, __sp1_remove_spr_char

asm_sp1_MoveSprAbs:

   ld (ix+5),b
   ld a,b

   cp (ix+17)
   rl b

   add a,a
   add a,SP1V_ROTTBL/256
   ld (ix+9),a

   xor a
   sub c
   bit 7,(ix+4)
   jp z, MoveSprAbs_onebytedef
   sub c
   set 7,c

.MoveSprAbs_onebytedef:

   ld (ix+4),c
   ld c,a

   ld a,(ix+4)
   and $07
   cp (ix+18)
   ld a,b
   rla
   ex af,af

   ld a,h
   or l
   jr nz, MoveSprAbs_newframe

   ld l,(ix+6)
   ld h,(ix+7)
   jp MoveSprAbs_framerejoin

.MoveSprAbs_newframe:

   ld (ix+6),l
   ld (ix+7),h

.MoveSprAbs_framerejoin:

   ld a,c
   or a
   jr z, MoveSprAbs_skipadj

   ld b,$ff
   add hl,bc

.MoveSprAbs_skipadj:

   ld (ix+11),l
   ld (ix+12),h

   ld (ix+19),0

   ld a,(ix+0)
   cp d
   jp nz, MoveSprAbs_changing0
   ld a,(ix+1)
   cp e
   jp nz, MoveSprAbs_changing1

   ld h,(ix+15)
   ld l,(ix+16)
   push de
   exx
   pop de
   ld hl,(SP1V_UPDATELISTT)
   ld bc,6
   add hl,bc
   push hl
   call asm_sp1_GetUpdateStruct
   ld b,(ix+0)
   pop de
   push hl
   push de

.MoveSprAbs_NCrowloop:

   ld a,b
   inc b

   sub (iy+0)
   jp c, MoveSprAbs_NCcliprow0
   sub (iy+3)
   jp nc, MoveSprAbs_NCcliprow0

   ld a,b
   sub (ix+0)
   cp (ix+3)
   jp nz, MoveSprAbs_NCnotlastrow

   ex af,af
   bit 0,a
   jp nz, MoveSprAbs_NCcliprow1
   ex af,af

.MoveSprAbs_NCnotlastrow:

   ld c,(ix+1)

.MoveSprAbs_NCcolloop:

   ld a,c
   inc c

   bit 6,(hl)
   ex (sp),hl
   jr nz, MoveSprAbs_NCclipcol0

   sub (iy+1)
   jr c, MoveSprAbs_NCclipcol0
   sub (iy+2)
   jr nc, MoveSprAbs_NCclipcol0

   ld a,c
   sub (ix+1)
   cp (ix+2)
   jp nz, MoveSprAbs_NCnotlastcol

   ex af,af
   bit 1,a
   jr nz, MoveSprAbs_NCclipcol1
   ex af,af

.MoveSprAbs_NCnotlastcol:

   exx
   push af
   inc (ix+19)

   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl

   ld a,(hl)
   or a
   jr z, MoveSprAbs_NCaddit

   pop bc
   pop hl

.MoveSprAbs_NCrejoinaddit:

   ld a,(hl)
   xor $80
   jp p, MoveSprAbs_NCalreadyinv0
   ld (hl),a

   push hl
   exx
   pop de
   ld (hl),d
   inc hl
   ld (hl),e
   ld hl,6
   add hl,de
   ld (hl),0
   exx

.MoveSprAbs_NCalreadyinv0:

   bit 6,c
   jr nz, MoveSprAbs_NCnextrow

.MoveSprAbs_NCnextcol:

   ld bc,10
   add hl,bc
   push hl
   ex de,hl
   exx
   ex (sp),hl

   jp MoveSprAbs_NCcolloop

.MoveSprAbs_NCclipcol1:

   ex af,af

.MoveSprAbs_NCclipcol0:

   exx

   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl

   ld a,(hl)
   or a
   jr nz, MoveSprAbs_NCremoveit

   inc hl
   inc hl
   inc hl

.MoveSprAbs_NCrejoinremove:

   bit 6,(hl)
   pop hl
   jr z, MoveSprAbs_NCnextcol

.MoveSprAbs_NCnextrow:

   pop hl

   ld a,d
   or a
   jp z, MoveSprAbs_done

   ld bc,10*SP1V_DISPWIDTH
   add hl,bc
   push hl
   push hl
   ex de,hl
   exx
   ex (sp),hl

   jp MoveSprAbs_NCrowloop

.MoveSprAbs_NCaddit:

   pop af
   ld b,d
   ld c,e
   pop de
   push de
   push af
   ld (hl),d
   inc hl
   ld (hl),e
   inc hl
   ld a,(hl)
   inc hl

   bit 7,(hl)
   jp z, MoveSprAbs_NCnotoccluding10
   ex de,hl
   inc (hl)
   ex de,hl

.MoveSprAbs_NCnotoccluding10:

   inc hl
   push bc
   ld b,h
   ld c,l
   ld hl,4
   add hl,de
   call __sp1_add_spr_char
   pop de
   pop bc
   pop hl

   jp MoveSprAbs_NCrejoinaddit

.MoveSprAbs_NCremoveit:

   push de
   ld (hl),0
   inc hl
   inc hl
   inc hl
   push hl
   inc hl
   call __sp1_remove_spr_char
   pop hl
   pop de

   pop bc
   push bc

   ld a,(bc)
   bit 7,(hl)
   jp z, MoveSprAbs_NCnotoccluding0
   dec a
   ld (bc),a

.MoveSprAbs_NCnotoccluding0:

   xor $80
   jp p, MoveSprAbs_NCrejoinremove
   ld (bc),a

   push bc
   exx
   pop de
   ld (hl),d
   inc hl
   ld (hl),e
   ld hl,6
   add hl,de
   ld (hl),0
   exx

   jp MoveSprAbs_NCrejoinremove

.MoveSprAbs_NCcliprow1:

   ex af,af

.MoveSprAbs_NCcliprow0:

   ex (sp),hl
   exx

.MoveSprAbs_NCcliprowlp:

   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl

   ld a,(hl)
   or a
   jr nz, MoveSprAbs_NCCRremoveit

   inc hl
   inc hl
   inc hl

.MoveSprAbs_NCCRrejoinremove:

   bit 6,(hl)
   pop hl
   jr nz, MoveSprAbs_NCCRnextrow

   ld bc,10
   add hl,bc
   push hl
   ex de,hl

   jp MoveSprAbs_NCcliprowlp

.MoveSprAbs_NCCRnextrow:

   pop hl

   ld a,d
   or a
   jp z, MoveSprAbs_done

   ld bc,10*SP1V_DISPWIDTH
   add hl,bc
   push hl
   push hl
   ex de,hl
   exx
   ex (sp),hl

   jp MoveSprAbs_NCrowloop

.MoveSprAbs_NCCRremoveit:

   push de
   ld (hl),0
   inc hl
   inc hl
   inc hl
   push hl
   inc hl
   call __sp1_remove_spr_char
   pop hl
   pop de

   pop bc
   push bc

   ld a,(bc)
   bit 7,(hl)
   jp z, MoveSprAbs_NCCRnotoccluding0
   dec a
   ld (bc),a

.MoveSprAbs_NCCRnotoccluding0:

   xor $80
   jp p, MoveSprAbs_NCCRrejoinremove
   ld (bc),a

   push bc
   exx
   pop de
   ld (hl),d
   inc hl
   ld (hl),e
   ld hl,6
   add hl,de
   ld (hl),0
   exx

   jp MoveSprAbs_NCCRrejoinremove

.MoveSprAbs_done:

   exx
   ld de,-6
   add hl,de
   ld (SP1V_UPDATELISTT),hl
   ret

.MoveSprAbs_changing0:

   ld (ix+0),d

.MoveSprAbs_changing1:

   ld (ix+1),e

   ld h,(ix+15)
   ld l,(ix+16)
   push de
   exx
   pop de
   ld hl,(SP1V_UPDATELISTT)
   ld bc,6
   add hl,bc
   push hl
   call asm_sp1_GetUpdateStruct
   ld b,(ix+0)
   pop de
   push hl
   push de

.MoveSprAbs_CCrowloop:

   ld a,b
   inc b

   sub (iy+0)
   jp c, MoveSprAbs_CCcliprow0
   sub (iy+3)
   jp nc, MoveSprAbs_CCcliprow0

   ld a,b
   sub (ix+0)
   cp (ix+3)
   jp nz, MoveSprAbs_CCnotlastrow

   ex af,af
   bit 0,a
   jp nz, MoveSprAbs_CCcliprow1
   ex af,af

.MoveSprAbs_CCnotlastrow:

   ld c,(ix+1)

.MoveSprAbs_CCcolloop:

   ld a,c
   inc c

   bit 6,(hl)
   ex (sp),hl
   jp nz, MoveSprAbs_CCclipcol0

   sub (iy+1)
   jp c, MoveSprAbs_CCclipcol0
   sub (iy+2)
   jp nc, MoveSprAbs_CCclipcol0

   ld a,c
   sub (ix+1)
   cp (ix+2)
   jp nz, MoveSprAbs_CCnotlastcol

   ex af,af
   bit 1,a
   jp nz, MoveSprAbs_CCclipcol1
   ex af,af

.MoveSprAbs_CCnotlastcol:

   exx
   inc (ix+19)

   ld b,(hl)
   inc hl
   ld c,(hl)
   inc hl

   ld a,(hl)
   or a
   jp z, MoveSprAbs_CCnoremovenec0

   push bc
   push hl
   ld bc,4
   add hl,bc
   call __sp1_remove_spr_char
   pop de
   pop hl
   ex (sp),hl
   ex de,hl

   ld b,(hl)
   inc hl
   ld c,(hl)
   ld (hl),e
   dec hl
   ld (hl),d

   inc hl
   inc hl
   inc hl
   bit 7,(hl)
   jp z, MoveSprAbs_CCnotoccl0
   ld a,(bc)
   dec a
   ld (bc),a
   ld a,(de)
   inc a
   ld (de),a

.MoveSprAbs_CCnotoccl0:

   ld a,(de)
   xor $80
   jp p, MoveSprAbs_CCnoinvnew
   ld (de),a

   push de
   exx
   pop de
   ld (hl),d
   inc hl
   ld (hl),e
   ld hl,6
   add hl,de
   ld (hl),0
   exx

.MoveSprAbs_CCnoinvnew:

   ld a,(bc)
   xor $80
   jp p, MoveSprAbs_CCnoinvold
   ld (bc),a

   push bc
   exx
   pop de
   ld (hl),d
   inc hl
   ld (hl),e
   ld hl,6
   add hl,de
   ld (hl),0
   exx

.MoveSprAbs_CCnoinvold:

   dec hl
   ld a,(hl)
   inc hl
   ld c,(hl)
   push bc
   inc hl
   ld b,h
   ld c,l
   ld hl,4
   add hl,de
   push de
   call __sp1_add_spr_char
   pop hl
   pop af
   pop de

   jr z, MoveSprAbs_CCnextrow

   ld bc,10
   add hl,bc
   push hl
   ex de,hl
   exx
   ex (sp),hl

   jp MoveSprAbs_CCcolloop

.MoveSprAbs_CCnextrow:

   pop hl

   ld a,d
   or a
   jp z, MoveSprAbs_done

   ld bc,10*SP1V_DISPWIDTH
   add hl,bc
   push hl
   push hl
   ex de,hl
   exx
   ex (sp),hl

   jp MoveSprAbs_CCrowloop

.MoveSprAbs_CCnoremovenec0:

   pop de
   ld (hl),d
   inc hl
   ld (hl),e
   inc hl
   inc hl

   ld a,(de)
   bit 7,(hl)
   jp z, MoveSprAbs_CCnotoccl12
   inc a
   ld (de),a

.MoveSprAbs_CCnotoccl12:

   xor $80
   jp p, MoveSprAbs_CCalreadyinv33
   ld (de),a

   push de
   exx
   pop de
   ld (hl),d
   inc hl
   ld (hl),e
   ld hl,6
   add hl,de
   ld (hl),0
   exx

.MoveSprAbs_CCalreadyinv33:

   push bc

   jp MoveSprAbs_CCnoinvold

.MoveSprAbs_CCclipcol1:

   ex af,af

.MoveSprAbs_CCclipcol0:

   exx

   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl

   ld a,(hl)
   or a
   jr z, MoveSprAbs_CCskipremoveit

   ld b,a
   inc hl
   ld c,(hl)

   push bc
   push de
   dec hl
   ld (hl),0
   inc hl
   inc hl
   inc hl
   push hl
   inc hl
   call __sp1_remove_spr_char
   pop hl
   pop de
   pop bc

   ld a,(bc)
   bit 7,(hl)
   jp z, MoveSprAbs_CCnotoccl44
   dec a
   ld (bc),a

.MoveSprAbs_CCnotoccl44:

   xor $80
   jp p, MoveSprAbs_CCalreadyinv66
   ld (bc),a

   push bc
   exx
   pop de
   ld (hl),d
   inc hl
   ld (hl),e
   ld hl,6
   add hl,de
   ld (hl),0
   exx

.MoveSprAbs_CCalreadyinv66:

   pop bc
   bit 6,(hl)
   jp nz, MoveSprAbs_CCnextrow

   ld hl,10
   add hl,bc
   push hl
   ex de,hl
   exx
   ex (sp),hl

   jp MoveSprAbs_CCcolloop

.MoveSprAbs_CCskipremoveit:

   inc hl
   inc hl
   inc hl

   jp MoveSprAbs_CCalreadyinv66

.MoveSprAbs_CCcliprow1:

   ex af,af

.MoveSprAbs_CCcliprow0:

   ex (sp),hl
   exx

.MoveSprAbs_CCcliprowlp:

   ld d,(hl)
   inc hl
   ld e,(hl)
   inc hl

   ld a,(hl)
   or a
   jr nz, MoveSprAbs_CCCRremoveit

   inc hl
   inc hl
   inc hl

.MoveSprAbs_CCCRrejoinremove:

   bit 6,(hl)
   pop hl
   jr nz, MoveSprAbs_CCCRnextrow

   ld bc,10
   add hl,bc
   push hl
   ex de,hl

   jp MoveSprAbs_CCcliprowlp

.MoveSprAbs_CCCRnextrow:

   pop hl

   ld a,d
   or a
   jp z, MoveSprAbs_done

   ld bc,10*SP1V_DISPWIDTH
   add hl,bc
   push hl
   push hl
   ex de,hl
   exx
   ex (sp),hl

   jp MoveSprAbs_CCrowloop

.MoveSprAbs_CCCRremoveit:

   ld b,a
   inc hl
   ld c,(hl)

   push bc
   push de
   dec hl
   ld (hl),0
   inc hl
   inc hl
   inc hl
   push hl
   inc hl
   call __sp1_remove_spr_char
   pop hl
   pop de
   pop bc

   ld a,(bc)
   bit 7,(hl)
   jp z, MoveSprAbs_CCCRnotoccluding0
   dec a
   ld (bc),a

.MoveSprAbs_CCCRnotoccluding0:

   xor $80
   jp p, MoveSprAbs_CCCRrejoinremove
   ld (bc),a

   push bc
   exx
   pop de
   ld (hl),d
   inc hl
   ld (hl),e
   ld hl,6
   add hl,de
   ld (hl),0
   exx

   jp MoveSprAbs_CCCRrejoinremove
