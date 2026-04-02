INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_IterateSprChar

EXTERN l_jpix

asm_sp1_IterateSprChar:

; Iterate over all the struct sp1_cs contained in a sprite
; in row major order, calling the user function for each one.
;
; enter : hl = & struct sp1_ss
;         ix = user function
; uses  : af, bc, hl + whatever user function uses

   ld bc,15
   add hl,bc              ; hl = & struct sp1_ss.first

   ld c,b                 ; bc = sprite char counter = 0

.IterateSprChar_iterloop:

   ld a,(hl)
   or a
   ret z

   inc hl
   ld l,(hl)
   ld h,a                 ; hl = & next struct sp1_cs

   push ix

   push hl                ; save &cs for loop restore
   push bc                ; save count for loop restore
   ld   d, h
   ld   e, l              ; DE = &cs   (sdcccall(1): 2nd ptr arg -> DE)
   ld   l, c
   ld   h, b              ; HL = count (sdcccall(1): 1st u16 arg -> HL)

   call l_jpix            ; call userfunc(uint count, struct sp1_cs *c)

   pop bc                 ; restore count
   pop hl                 ; restore &cs

   pop ix

   inc bc
   jp IterateSprChar_iterloop
