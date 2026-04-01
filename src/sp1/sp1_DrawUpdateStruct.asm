INCLUDE "sp1___config.asm"

PUBLIC SP1DrawUpdateStruct
PUBLIC SP1RETSPRDRAW

.DrawUpdateStruct_haveocclspr

   inc hl
   push hl
   dec hl
   push de
   jp DrawUpdateStruct_skiplp

.DrawUpdateStruct_skipthisone

   ld hl,15
   add hl,de

.DrawUpdateStruct_skiplp

   ld d,(hl)
   inc hl
   ld e,(hl)
   dec de
   ld a,(de)
   rla
   jr nc, DrawUpdateStruct_skipthisone

   djnz DrawUpdateStruct_skipthisone

   and $20
   pop hl
   jr z, DrawUpdateStruct_noclearbuff

   ld a,h
   or a
   jr nz, DrawUpdateStruct_havetiledef2

   ld h,SP1V_TILEARRAY/256
   ld a,(hl)
   inc h
   ld h,(hl)
   ld l,a

.DrawUpdateStruct_havetiledef2

   push de
   ld de,SP1V_PIXELBUFFER
   ldi
   ldi
   ldi
   ldi
   ldi
   ldi
   ldi
   ld a,(hl)
   ld (de),a
   pop de

.DrawUpdateStruct_noclearbuff

   ex de,hl
   inc hl
   jp DrawUpdateStruct_spritedraw

SP1DrawUpdateStruct:

   inc hl
   ld a,(hl)
   ld (SP1V_ATTRBUFFER),a
   inc hl
   ld e,(hl)
   inc hl
   ld d,(hl)
   inc hl

   djnz DrawUpdateStruct_haveocclspr

   ex de,hl

   ld a,h
   or a
   jr nz, DrawUpdateStruct_havetiledef

   ld h,SP1V_TILEARRAY/256
   ld a,(hl)
   inc h
   ld h,(hl)
   ld l,a

.DrawUpdateStruct_havetiledef

   ld a,(de)
   or a
   jr z, DrawUpdateStruct_drawtileonly

   push de
   ld de,SP1V_PIXELBUFFER
   ldi
   ldi
   ldi
   ldi
   ldi
   ldi
   ldi
   ld a,(hl)
   ld (de),a
   pop hl

   ld a,(hl)
   inc hl
   push hl

.DrawUpdateStruct_spritedrawlp

   ld l,(hl)
   ld h,a

.DrawUpdateStruct_spritedraw

   ld a,(SP1V_ATTRBUFFER)
   and (hl)
   inc hl
   or (hl)
   inc hl
   ld (SP1V_ATTRBUFFER),a

   ld e,(hl)
   inc hl
   ld d,(hl)
   inc hl
   ex de,hl

   jp (hl)

.DrawUpdateStruct_drawtileonly

   ex de,hl
   inc hl
   inc hl
   ld b,(hl)
   inc hl
   ld c,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld h,(hl)
   ld l,a

   ld a,(de)
   ld (hl),a
   inc h
   inc de

   ld a,(de)
   ld (hl),a
   inc h
   inc de

   ld a,(de)
   ld (hl),a
   inc h
   inc de

   ld a,(de)
   ld (hl),a
   inc h
   inc de

   ld a,(de)
   ld (hl),a
   inc h
   inc de

   ld a,(de)
   ld (hl),a
   inc h
   inc de

   ld a,(de)
   ld (hl),a
   inc h
   inc de

   ld a,(de)
   ld (hl),a

   ld a,h
   xor $85
   rrca
   rrca
   rrca
   ld h,a

   ld a,(SP1V_ATTRBUFFER)
   ld (hl),a

   ret

SP1RETSPRDRAW:

   pop hl
   ld a,(hl)
   inc hl
   or a
   jr nz, DrawUpdateStruct_spritedrawlp

   pop hl

.DrawUpdateStruct_donesprites

   inc hl
   ld b,(hl)
   inc hl
   ld c,(hl)
   inc hl
   ld a,(hl)
   inc hl
   ld h,(hl)
   ld l,a

   ld de,(SP1V_PIXELBUFFER+0)
   ld (hl),e
   inc h
   ld (hl),d
   inc h
   ld de,(SP1V_PIXELBUFFER+2)
   ld (hl),e
   inc h
   ld (hl),d
   inc h
   ld de,(SP1V_PIXELBUFFER+4)
   ld (hl),e
   inc h
   ld (hl),d
   inc h
   ld de,(SP1V_PIXELBUFFER+6)
   ld (hl),e
   inc h
   ld (hl),d

.DrawUpdateStruct_rejoin

   ld a,h
   xor $85
   rrca
   rrca
   rrca
   ld h,a

   ld a,(SP1V_ATTRBUFFER)
   ld (hl),a

   ret
