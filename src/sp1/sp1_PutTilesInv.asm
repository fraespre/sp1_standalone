INCLUDE "sp1___config.asm"

SECTION _CODE_SP1

PUBLIC asm_sp1_PutTilesInv

EXTERN asm_sp1_GetUpdateStruct

asm_sp1_PutTilesInv:

; Copy a rectangular set of tiles and colours to screen and invalidate
; the rectangular area so that it is drawn in the next update.
;
; enter : hl = struct sp1_tp[] array of attr/tile pairs
;          d = row coord
;          e = col coord
;          b = width
;          c = height
; uses  : af, bc, de, hl, af_alt, ix

   push ix                               ; save caller's IX (SDCC uses as frame pointer)
   push hl
   call asm_sp1_GetUpdateStruct          ; hl = & struct sp1_update
   pop de                                ; de = struct sp1_tp *
   ex de,hl                              ; hl = struct sp1_tp *, de = & struct sp1_update

   ld a,c                                ; a = height
   ld c,$ff

   ld ix,(SP1V_UPDATELISTT)

.rowloop

   push bc                            ; save b = width
   push de                            ; save update position
   ex af,af                           ; a_alt = height

.colloop

   ld a,(de)
   xor $80
   jp p, skipinval                    ; bit 7 now reset if already invalidated
   ld (de),a

   ld (ix+6),d                        ; this struct sp1_update to end of list
   ld (ix+7),e
   push de
   pop ix

.skipinval

   inc de
   ldi                                ; copy colour and tile from struct sp1_tp[]

   ldi                                ; into struct sp1_update
   ldi
   ld a,6
   add a,e
   ld e,a
   jp nc, noinc
   inc d                              ; de = next struct sp1_update * one column to right

.noinc

   djnz colloop

   ex (sp),hl                         ; hl = struct sp1_update * in same row but leftmost column
   ld bc,10*SP1V_DISPWIDTH
   add hl,bc                          ; hl = struct sp1_update * one row down leftmost column
   pop de
   ex de,hl                           ; de = struct sp1_update * down one row, hl = struct sp1_tp[]

   pop bc                             ; b = width

   ex af,af                           ; a = height
   dec a
   jp nz, rowloop

   ld (ix+6),0
   ld (SP1V_UPDATELISTT),ix

   pop ix                             ; restore caller's IX
   ret
