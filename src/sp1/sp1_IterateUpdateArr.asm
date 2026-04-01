INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_IterateUpdateArr

EXTERN l_jpix

asm_sp1_IterateUpdateArr:

; Iterate over a zero-terminated array of struct sp1_update*, calling the
; supplied function for each entry.
;
; enter : hl = zero-terminated array of struct sp1_update *
;         ix = function to call for each struct sp1_update * (called with hl = ptr)
; uses  : af, de, hl + whatever user function uses

.IterateUpdateArr_loop:

   ld e,(hl)
   inc hl
   ld d,(hl)             ; de = struct sp1_update *

   ld a,d
   or e
   ret z                 ; null terminator -- done

   inc hl
   push ix
   push hl
   push de
   ex de,hl
   call l_jpix           ; call function with hl = struct sp1_update *
   pop de
   pop hl
   pop ix

   jp IterateUpdateArr_loop
