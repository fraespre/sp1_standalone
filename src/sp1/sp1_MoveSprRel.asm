SECTION _CODE_SP1

PUBLIC asm_sp1_MoveSprRel

EXTERN asm_sp1_MoveSprAbs

asm_sp1_MoveSprRel:

; Move sprite a relative distance from current position.
;
; enter: IX = sprite structure address
;        HL = next sprite frame address (0 for no change)
;         D = relative row coord, signed byte
;         E = relative col coord, signed byte
;         B = relative horizontal pixel movement, signed byte
;         C = relative vertical pixel movement, signed byte
;        IY = clipping rectangle
; exits via asm_sp1_MoveSprAbs

   ld a,(ix+5)           ; current horizontal rotation
   add a,b
   ld b,a
   sra a
   sra a
   sra a
   add a,e
   add a,(ix+1)
   ld e,a                ; e = absolute column position
   ld a,b
   cp 0x80
   jp c, mvpos1
   add a,8

.mvpos1

   and 0x07
   ld b,a                ; b = absolute horizontal rotation
   ld a,(ix+4)           ; current vertical rotation
   and 0x07              ; get rid of flag in bit 7
   add a,c
   ld c,a
   sra a
   sra a
   sra a
   add a,d
   add a,(ix+0)
   ld d,a                ; d = absolute row position
   ld a,c
   cp 0x80
   jp c, mvpos2
   add a,8

.mvpos2

   and 0x07
   ld c,a                ; c = absolute vertical rotation

   jp asm_sp1_MoveSprAbs
