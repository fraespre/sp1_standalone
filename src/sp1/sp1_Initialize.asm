INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_Initialize

asm_sp1_Initialize:

   push hl

   bit 0,a
   jr z, norottbl

   ld c,7
   push af

.rottbllp

   ld a,c
   add a,a
   or SP1V_ROTTBL/256
   ld h,a
   ld l,0

.entrylp

   ld b,c
   ld e,l
   xor a

.rotlp

   srl e
   rra
   djnz rotlp

   ld (hl),e
   inc h
   ld (hl),a
   dec h
   inc l
   jp nz, entrylp

   dec c
   jp nz, rottbllp
   pop af

.norottbl

   ld hl,SP1V_TILEARRAY
   ld de,15360
   ld b,0
   ld c,a

.tileloop

   ld a,(hl)
   inc h
   bit 1,c
   jr nz, overwrite
   or (hl)
   jr nz, tilepresent

.overwrite

   ld (hl),d
   dec h
   ld (hl),e
   inc h

.tilepresent

   dec h
   inc hl

   ld a,8
   add a,e
   ld e,a
   ld a,0
   adc a,d
   ld d,a

   djnz tileloop

   ld hl,SP1V_UPDATELISTH
   ld (SP1V_UPDATELISTT),hl
   ld hl,0
   ld (SP1V_UPDATELISTH+6),hl

   pop de
   ld b,SP1V_DISPORIGY
   ld hl,SP1V_UPDATEARRAY
   bit 2,c
   ex af,af

.rowloop

   ld c,SP1V_DISPORIGX

.colloop

   ld (hl),1
   inc hl
   ld (hl),d
   inc hl
   ld (hl),e
   inc hl
   ld (hl),0
   inc hl
   ld (hl),0
   inc hl
   ld (hl),0
   inc hl
   ld (hl),0
   inc hl
   ld (hl),0
   inc hl

   ex af,af
   jr z, skipscrnaddr
   ex af,af

   ld a,b
   rrca
   rrca
   rrca
   and $e0
   or c
   ld (hl),a
   inc hl
   ld a,b
   and $18
   or $40
   ld (hl),a

.rejoinscrnaddr

   inc hl
   inc c
   ld a,c
   cp SP1V_DISPORIGX + SP1V_DISPWIDTH
   jr c, colloop

   inc b
   ld a,b
   cp SP1V_DISPORIGY + SP1V_DISPHEIGHT
   jr c, rowloop

   ret

.skipscrnaddr

   ex af,af
   inc hl
   jp rejoinscrnaddr
