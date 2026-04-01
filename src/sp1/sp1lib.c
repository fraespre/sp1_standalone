#include "sp1lib.h"

// =========================================================
// UPDATER WRAPPERS
// sdcccall(1) convention:
//   1st u8  -> A          1st ptr/u16 -> HL
//   2nd u8  -> L          2nd ptr/u16 -> DE
//   3+ u8   -> stack      3rd ptr/u16 -> BC
//   (consecutive u8 stack args packed as words: low=earlier, high=later)
// =========================================================

// void sp1_UpdateNow(void)
void sp1_UpdateNow(void) __naked {
   __asm
      EXTERN asm_sp1_UpdateNow
      jp   asm_sp1_UpdateNow
   __endasm;
}

// void sp1_Initialize(u8 iflag, u8 colour, u8 tile)
// sdcc: A=iflag, L=colour, [SP+2]=tile
// asm:  A=iflag, H=colour, L=tile
void sp1_Initialize(uint8_t iflag, uint8_t colour, uint8_t tile) __naked __sdcccall(1) {
   iflag; colour; tile;
   __asm
      EXTERN asm_sp1_Initialize
      defb 0x08              ; ex af,af_alt -> A_alt=iflag
      ld   d, l             ; D=colour
      ld   hl, 2
      add  hl, sp           ; HL=&tile
      ld   l, (hl)          ; L=tile
      ld   h, d             ; H=colour
      defb 0x08              ; ex af,af_alt -> A=iflag
      call asm_sp1_Initialize
      ; callee-cleanup: remove 1 stack byte (tile)
      pop  hl               ; HL=return address, SP+=2
      inc  sp               ; SP+=1 (tile cleaned)
      jp   (hl)             ; return to caller
   __endasm;
}


// struct sp1_update *sp1_GetUpdateStruct(u8 row, u8 col)
// sdcc: A=row, L=col
// asm:  D=row, E=col -> returns HL=ptr
// SDCC optimizer sometimes uses DE instead of HL for the return value when HL is
// immediately repurposed for the store destination (e.g. star_list[i] = sp1_GetUpdateStruct(...)).
// Fix: copy result to DE as well so both HL and DE = ptr after return.
struct sp1_update *sp1_GetUpdateStruct(uint8_t row, uint8_t col) __naked __sdcccall(1) {
   row; col;
   __asm
      EXTERN asm_sp1_GetUpdateStruct
      ld   d, a
      ld   e, l
      call asm_sp1_GetUpdateStruct  ; HL = update struct ptr
      ld   d, h                     ; DE = ptr (for SDCC store-to-mem optimization)
      ld   e, l
      ret
   __endasm;
}

// void sp1_InvUpdateStruct(struct sp1_update *u)
// sdcc: HL=u  asm: HL=u
void sp1_InvUpdateStruct(struct sp1_update *u) __naked __sdcccall(1) {
   u;
   __asm
      EXTERN asm_sp1_InvUpdateStruct
      jp   asm_sp1_InvUpdateStruct
   __endasm;
}

// void sp1_IterateUpdateArr(struct sp1_update **ua, void *hook)
// sdcc: HL=ua, DE=hook  asm: HL=array, IX=hook
// z88dk version uses stack-based convention (pop af/hl/ix) -- incompatible, must override
void sp1_IterateUpdateArr(struct sp1_update **ua, void *hook) __naked __sdcccall(1) {
   ua; hook;
   __asm
      EXTERN asm_sp1_IterateUpdateArr
      push ix
      push de
      pop  ix              ; IX = hook
                           ; HL = ua already
      call asm_sp1_IterateUpdateArr
      pop  ix              ; restore caller IX
      ret
   __endasm;
}

// void sp1_Invalidate(struct sp1_Rect *r)
// sdcc: HL=r  asm: D=row, E=col, B=width, C=height (loaded from struct)
void sp1_Invalidate(struct sp1_Rect *r) __naked __sdcccall(1) {
   r;
   __asm
      EXTERN asm_sp1_Invalidate
      ld   d, (hl)
      inc  hl
      ld   e, (hl)
      inc  hl
      ld   b, (hl)
      inc  hl
      ld   c, (hl)
      jp   asm_sp1_Invalidate
   __endasm;
}

// =========================================================
// TILE WRAPPERS
// =========================================================

// void *sp1_TileEntry(u8 c, void *def)
// sdcc: A=c, DE=def  (SDCC puts ptr in DE when first arg is u8)
// asm:  C=c, DE=def
void *sp1_TileEntry(uint8_t c, void *def) __naked __sdcccall(1) {
   c; def;
   __asm
      EXTERN asm_sp1_TileEntry
      ld   c, a             ; C=c, DE already=def
      jp   asm_sp1_TileEntry
   __endasm;
}

// void sp1_PrintAt(u8 row, u8 col, u8 colour, u16 tile)
// sdcc: A=row, L=col, [SP+2]=colour, [SP+3..4]=tile
// asm:  D=row, E=col, A=colour, BC=tile
// Stack layout: push bc(tile=2B) then push af+inc sp(colour=1B) -> [SP+2]=colour,[SP+3]=tile_lo,[SP+4]=tile_hi
void sp1_PrintAt(uint8_t row, uint8_t col, uint8_t colour, uint16_t tile) __naked __sdcccall(1) {
   row; col; colour; tile;
   __asm
      EXTERN asm_sp1_PrintAt
      ld   d, a
      ld   e, l
      ld   hl, 2
      add  hl, sp           ; HL=&colour
      ld   a, (hl)          ; A=colour
      inc  hl               ; HL=&tile_lo
      ld   c, (hl)
      inc  hl               ; HL=&tile_hi
      ld   b, (hl)          ; BC=tile
      call asm_sp1_PrintAt
      pop  hl               ; HL = caller return address
      inc  sp               ; skip colour byte
      inc  sp               ; skip tile_lo byte
      inc  sp               ; skip tile_hi byte
      jp   (hl)             ; return to caller with stack cleaned
   __endasm;
}

// void sp1_ClearRect(struct sp1_Rect *r, u8 colour, u8 tile, u8 rflag)
// sdcc: HL=r, A=tile(unreliable), [SP+2]=colour, [SP+3]=tile, [SP+4]=rflag
// asm:  A=rflag, H=colour, L=tile, D=row, E=col, B=width, C=height
// Note: SDCC packs colour(E) and tile(D) via push de; A at entry is tile, NOT colour
void sp1_ClearRect(struct sp1_Rect *r, uint8_t colour, uint8_t tile, uint8_t rflag) __naked __sdcccall(1) {
   r; colour; tile; rflag;
   __asm
      EXTERN asm_sp1_ClearRect
      ld   d, (hl)
      inc  hl
      ld   e, (hl)
      inc  hl
      ld   b, (hl)
      inc  hl
      ld   c, (hl)          ; DEBC=row,col,width,height
      ld   hl, 4
      add  hl, sp           ; HL=SP+4=&rflag
      ld   a, (hl)          ; A=rflag
      defb 0x08             ; ex af,af_alt -> A_alt=rflag
      dec  hl               ; HL=SP+3=&tile
      ld   a, (hl)          ; A=tile
      dec  hl               ; HL=SP+2=&colour
      ld   h, (hl)          ; H=colour
      ld   l, a             ; L=tile
      defb 0x08             ; ex af,af_alt -> A=rflag
      push iy
      push ix               ; save IX: SP1CRSELECT clobbers IX (loads draw fn ptr via ld ixl/ixh)
      call asm_sp1_ClearRect
      pop  ix               ; restore IX
      pop  iy
      ; callee-cleanup: remove 3 stack bytes (colour=1, tile=1, rflag=1)
      pop  hl               ; HL=return address, SP+=2
      inc  sp
      inc  sp
      inc  sp               ; SP+=3 (args cleaned)
      jp   (hl)             ; return to caller
   __endasm;
}

// void sp1_ClearRectInv(struct sp1_Rect *r, u8 colour, u8 tile, u8 rflag)
// sdcc: HL=r, A=tile(unreliable), [SP+2]=colour, [SP+3]=tile, [SP+4]=rflag
// asm:  A=rflag, H=colour, L=tile, D=row, E=col, B=width, C=height
// Note: SDCC packs colour(E) and tile(D) via push de; A at entry is tile, NOT colour
void sp1_ClearRectInv(struct sp1_Rect *r, uint8_t colour, uint8_t tile, uint8_t rflag) __naked __sdcccall(1) {
   r; colour; tile; rflag;
   __asm
      EXTERN asm_sp1_ClearRectInv
      ld   d, (hl)
      inc  hl
      ld   e, (hl)
      inc  hl
      ld   b, (hl)
      inc  hl
      ld   c, (hl)          ; DEBC=row,col,width,height
      ld   hl, 4
      add  hl, sp           ; HL=SP+4=&rflag
      ld   a, (hl)          ; A=rflag
      defb 0x08             ; ex af,af_alt -> A_alt=rflag
      dec  hl               ; HL=SP+3=&tile
      ld   a, (hl)          ; A=tile
      dec  hl               ; HL=SP+2=&colour
      ld   h, (hl)          ; H=colour
      ld   l, a             ; L=tile
      defb 0x08             ; ex af,af_alt -> A=rflag
      push iy
      push ix               ; save IX: SP1CRSELECT clobbers IX (loads draw fn ptr via ld ixl/ixh)
      call asm_sp1_ClearRectInv
      pop  ix               ; restore IX
      pop  iy
      ; callee-cleanup: remove 3 stack bytes (colour=1, tile=1, rflag=1)
      pop  hl               ; HL=return address, SP+=2
      inc  sp
      inc  sp
      inc  sp               ; SP+=3 (args cleaned)
      jp   (hl)             ; return to caller
   __endasm;
}

// =========================================================
// SPRITE WRAPPERS
// =========================================================

// struct sp1_ss *sp1_CreateSpr(void *drawf, u8 type, u8 height, u16 graphic, u8 plane)
// sdcc ACTUAL: HL=drawf, DE=&sprtbl[i] (caller temp, unused), A=plane
//   Stack: [ret(2), type(1), height(1), graphic(2), plane(1), &sprtbl(2)]
// asm:  A=height, B=type, C=plane, DE=drawf, HL=graphic  retval: HL->DE
struct sp1_ss *sp1_CreateSpr(void *drawf, uint8_t type, uint8_t height, uint16_t graphic, uint8_t plane) __naked __sdcccall(1) {
   drawf; type; height; graphic; plane;
   __asm
      EXTERN asm_sp1_CreateSpr
      ld   c, a             ; C=plane (from A)
      push hl               ; save drawf. After push: SP+0,1=drawf SP+2,3=ret SP+4=type SP+5=height SP+6,7=graphic SP+8=plane SP+9,10=&sprtbl
      ld   hl, 4
      add  hl, sp           ; HL=SP+4=&type
      ld   b, (hl)          ; B=type (SP+4)
      inc  hl
      ld   a, (hl)          ; A=height (SP+5)
      inc  hl
      ld   e, (hl)          ; E=graphic_lo (SP+6)
      inc  hl
      ld   d, (hl)          ; D=graphic_hi (SP+7) -> DE=graphic
      pop  hl               ; HL=drawf
      ex   de, hl           ; HL=graphic, DE=drawf
      call asm_sp1_CreateSpr  ; returns sprite ptr in DE; asm saves/restores IX and IY internally
      ex   de, hl           ; HL=sprite ptr (sdcccall return value)
      ; callee-cleanup: remove 5 stack bytes (type=1, height=1, graphic=2, plane=1)
      pop  hl               ; HL=return address, SP+=2
      inc  sp
      inc  sp
      inc  sp
      inc  sp
      inc  sp               ; SP+=5 (args cleaned)
      jp   (hl)             ; return to caller
   __endasm;
}

// uint16_t sp1_AddColSpr(struct sp1_ss *s, void *drawf, u8 type, u16 graphic, u8 plane)
// sdcc: HL=s, DE=drawf, A=type, [SP+2]=type(dup), [SP+3..4]=graphic, [SP+5]=plane
// asm:  IX=s, DE=drawf, H=plane, L=type, BC=graphic
uint16_t sp1_AddColSpr(struct sp1_ss *s, void *drawf, uint8_t type, uint16_t graphic, uint8_t plane) __naked __sdcccall(1) {
   s; drawf; type; graphic; plane;
   __asm
      EXTERN asm_sp1_AddColSpr
      push ix               ; save caller IX (frame pointer)
      push hl               ; push s temporarily
      ; Stack: [s, IX_saved, retaddr, type_dup, graphic_lo, graphic_hi, plane, ...]
      ld   hl, 7
      add  hl, sp           ; HL=SP+7=&graphic_lo
      ld   c, (hl)          ; C=graphic_lo
      inc  hl
      ld   b, (hl)          ; B=graphic_hi -> BC=graphic
      inc  hl
      ld   h, (hl)          ; H=[SP+9]=plane
      ld   l, a             ; L=type
      pop  ix               ; IX=s (from push hl above)
      push iy
      call asm_sp1_AddColSpr
      pop  iy
      pop  ix               ; restore caller IX (frame pointer)
      ; callee-cleanup: remove 4 stack bytes (type_dup=1, graphic=2, plane=1)
      pop  hl               ; HL=return address, SP+=2
      inc  sp
      inc  sp
      inc  sp
      inc  sp               ; SP+=4 (args cleaned)
      jp   (hl)             ; return to caller
   __endasm;
}

// void sp1_DeleteSpr(struct sp1_ss *s)
// sdcc: HL=s  asm: HL=s
void sp1_DeleteSpr(struct sp1_ss *s) __naked __sdcccall(1) {
   s;
   __asm
      EXTERN asm_sp1_DeleteSpr
      jp   asm_sp1_DeleteSpr
   __endasm;
}

// void sp1_MoveSprAbs(struct sp1_ss *s, struct sp1_Rect *clip, void *frame,
//                    u8 row, u8 col, u8 vrot, u8 hrot)
// sdcc ACTUAL: HL=s, DE=clip, A=row (from half-push trick, still in A at call)
//   Stack at entry: [ret(2), frame(2), row(1), col(1), vrot(1), hrot(1)]
//   (SDCC pushes frame to stack, not BC, for this 7-arg function)
// asm:  IX=s, IY=clip, HL=frame, D=row, E=col, B=hrot, C=vrot
// Callee cleans 6 stack bytes: frame(2)+row(1)+col(1)+vrot(1)+hrot(1)
void sp1_MoveSprAbs(struct sp1_ss *s, struct sp1_Rect *clip, void *frame,
                    uint8_t row, uint8_t col, uint8_t vrot, uint8_t hrot) __naked __sdcccall(1) {
   s; clip; frame; row; col; vrot; hrot;
   __asm
      EXTERN asm_sp1_MoveSprAbs
      ; Entry: HL=s, DE=clip, A=row
      ; Stack: [ret(2), frame(2), row(1), col(1), vrot(1), hrot(1)]
      push ix               ; save caller IX (SDCC frame pointer)
      push hl
      pop  ix               ; IX=s
      push de               ; push clip
      ex   (sp), iy         ; IY=clip, old IY saved
      ; Stack now (SP = entry_SP - 4):
      ; SP+0,1=old_IY  SP+2,3=saved_IX  SP+4,5=ret  SP+6,7=frame  SP+8=row  SP+9=col  SP+10=vrot  SP+11=hrot
      ld   hl, 9
      add  hl, sp
      ld   e, (hl)          ; E=col (SP+9)
      inc  hl
      ld   c, (hl)          ; C=vrot (SP+10)
      inc  hl
      ld   b, (hl)          ; B=hrot (SP+11)
      ld   d, a             ; D=row (A still holds row from call site)
      ld   hl, 6
      add  hl, sp
      ld   a, (hl)          ; A=frame_lo (SP+6)
      inc  hl
      ld   h, (hl)          ; H=frame_hi (SP+7)
      ld   l, a             ; HL=frame
      call asm_sp1_MoveSprAbs
      pop  iy               ; restore library IY
      pop  ix               ; restore caller IX (SDCC frame pointer)
      ; callee-cleanup: 6 stack bytes (frame=2, row=1, col=1, vrot=1, hrot=1)
      pop  hl               ; HL=ret addr
      inc  sp
      inc  sp
      inc  sp
      inc  sp
      inc  sp
      inc  sp
      jp   (hl)
   __endasm;
}

// void sp1_MoveSprRel(struct sp1_ss *s, struct sp1_Rect *clip, void *frame,
//                    i8 rel_row, i8 rel_col, i8 rel_vrot, i8 rel_hrot)
// sdcc ACTUAL: HL=s, DE=clip, A=rel_row
//   Stack: [ret(2), frame(2), rel_row(1), rel_col(1), rel_vrot(1), rel_hrot(1)]
//   rel_vrot/rel_hrot packed together by SDCC push de: E=rel_vrot at SP+6, D=rel_hrot at SP+7
// asm:  IX=s, IY=clip, HL=frame, D=rel_row, E=rel_col, B=rel_hrot, C=rel_vrot
// Callee cleans 6 stack bytes: frame(2)+rel_row(1)+rel_col(1)+rel_vrot(1)+rel_hrot(1)
void sp1_MoveSprRel(struct sp1_ss *s, struct sp1_Rect *clip, void *frame,
                    int8_t rel_row, int8_t rel_col, int8_t rel_vrot, int8_t rel_hrot) __naked __sdcccall(1) {
   s; clip; frame; rel_row; rel_col; rel_vrot; rel_hrot;
   __asm
      EXTERN asm_sp1_MoveSprRel
      ; Entry: HL=s, DE=clip, A=rel_row
      ; Stack: [ret(2), frame(2), rel_row(1), rel_col(1), rel_vrot(1 at SP+6), rel_hrot(1 at SP+7)]
      push ix               ; save caller IX (SDCC frame pointer)
      push hl
      pop  ix               ; IX=s
      push de               ; push clip
      ex   (sp), iy         ; IY=clip, old IY saved
      ; Stack now (SP = entry_SP - 4):
      ; SP+0,1=old_IY  SP+2,3=saved_IX  SP+4,5=ret  SP+6,7=frame  SP+8=rel_row  SP+9=rel_col  SP+10=rel_vrot  SP+11=rel_hrot
      ld   hl, 9
      add  hl, sp
      ld   e, (hl)          ; E=rel_col (SP+9)
      inc  hl
      ld   c, (hl)          ; C=rel_vrot (SP+10)
      inc  hl
      ld   b, (hl)          ; B=rel_hrot (SP+11)
      ld   d, a             ; D=rel_row (A still holds rel_row from call site)
      ld   hl, 6
      add  hl, sp
      ld   a, (hl)          ; A=frame_lo (SP+6)
      inc  hl
      ld   h, (hl)          ; H=frame_hi (SP+7)
      ld   l, a             ; HL=frame
      call asm_sp1_MoveSprRel
      pop  iy               ; restore library IY
      pop  ix               ; restore caller IX (SDCC frame pointer)
      ; callee-cleanup: 6 stack bytes (frame=2, rel_row=1, rel_col=1, rel_vrot=1, rel_hrot=1)
      pop  hl               ; HL=ret addr
      inc  sp
      inc  sp
      inc  sp
      inc  sp
      inc  sp
      inc  sp
      jp   (hl)
   __endasm;
}

// void sp1_MoveSprPix(struct sp1_ss *s, struct sp1_Rect *clip, void *frame,
//                    u16 x, u16 y)
// asm:  IX=s, IY=clip, HL=frame, DE=x, BC=y
// Callee cleans 6 stack bytes (frame=2, x=2, y=2).
//
// SDCC always pushes frame, x, y onto the stack (6 bytes callee-cleanup).
// BC may hold frame or x depending on register allocation, but is ignored.
// Stack layout at entry (both call sites): [ret | frame | x | y]
void sp1_MoveSprPix(struct sp1_ss *s, struct sp1_Rect *clip, void *frame,
                    uint16_t x, uint16_t y) __naked __sdcccall(1) {
   s; clip; frame; x; y;
   __asm
      EXTERN asm_sp1_MoveSprPix
      push ix               ; save callers IX (SDCC frame pointer)
      push hl
      pop  ix               ; IX=s
      push de
      ex   (sp), iy         ; IY=clip. Stack: [old_IY, saved_IX, ret, frame, x, y]
                            ; SP+0=old_IY, SP+2=saved_IX, SP+4=ret, SP+6=frame, SP+8=x, SP+10=y
      ld   hl, 10
      add  hl, sp
      ld   c, (hl)
      inc  hl
      ld   b, (hl)          ; BC=y
      ld   hl, 8
      add  hl, sp
      ld   e, (hl)
      inc  hl
      ld   d, (hl)          ; DE=x
      ld   hl, 6
      add  hl, sp
      ld   a, (hl)
      inc  hl
      ld   h, (hl)
      ld   l, a             ; HL=frame
      call asm_sp1_MoveSprPix  ; IX=s, IY=clip, HL=frame, DE=x, BC=y
      pop  iy               ; restore IY. Stack: [saved_IX, ret, frame, x, y]
      pop  ix               ; restore callers IX. Stack: [ret, frame, x, y]
      ; callee-cleanup: remove 6 stack bytes (frame=2, x=2, y=2)
      pop  hl               ; HL=return address. Stack: [frame, x, y]
      inc  sp
      inc  sp               ; skip frame
      inc  sp
      inc  sp               ; skip x
      inc  sp
      inc  sp               ; skip y
      jp   (hl)
   __endasm;
}


// void sp1_CreateCharStruct(sp1_cs *pTChar, u8 *addr, u8 x, u8 y, u8 plane)
// sdcccall(1) actual entry (SDCC adjacency trick pushes x+y together as one word):
//   HL=pTChar, DE=addr, A=y_or_x (unreliable, ignored)
//   stack=[retaddr(2), screen_y(SP+2), screen_x(SP+3), plane(SP+4)]  <- 3 bytes
// Callee cleans: 3 stack bytes
void sp1_CreateCharStruct(struct sp1_cs *pTChar, uint8_t *addr, uint8_t x, uint8_t y, uint8_t plane) __naked __sdcccall(1) {
   pTChar; addr; x; y; plane;
   __asm
      push hl                     ; save pTChar. Stack: [pTChar(2), retaddr(2), y(SP+4), x(SP+5), plane(SP+6)]
      ;
      ld   b, d
      ld   c, e                   ; BC = addr (graphic)
      ;
      ; Load y and x from stack (SP+4 and SP+5 after one push)
      ld   hl, 4
      add  hl, sp
      ld   d, (hl)                ; D = y
      inc  hl
      ld   e, (hl)                ; E = x  ->  DE = {y(hi), x(lo)} for push
      push de                     ; save {y,x}. Stack: [x(SP+0),y(SP+1), pTChar(2), retaddr(2), y(SP+6),x(SP+7),plane(SP+8)]
      ;
      ; Load plane (at SP+8 after two pushes) into A_alt
      ld   hl, 8
      add  hl, sp
      ld   a, (hl)                ; A = plane
      defb 0x08                   ; ex af,af_alt: A_alt = plane
      ;
      ; Load HL = pTChar (at SP+2)
      ld   hl, 2
      add  hl, sp
      ld   a, (hl)                ; A = pTChar_lo (temp)
      inc  hl
      ld   h, (hl)
      ld   l, a                   ; HL = pTChar (cs)
      ;
      EXTERN _SP1_DRAW_MASK2NR
      ld   de, _SP1_DRAW_MASK2NR  ; DE = drawf
      ld   a, 0x40                ; A = SP1_TYPE_2BYTE (must reload: was clobbered by HL construction)
      EXTERN asm_sp1_InitCharStruct
      call asm_sp1_InitCharStruct
      ;
      ; asm_sp1_GetUpdateStruct(D=y, E=x) -> HL=update_ptr
      pop  de                     ; pop de: E=x(SP+0), D=y(SP+1). Stack: [pTChar(2), retaddr(2), y,x,plane]
      EXTERN asm_sp1_GetUpdateStruct
      call asm_sp1_GetUpdateStruct ; HL = update_ptr
      ;
      ; asm_sp1_InsertCharStruct(HL=cs, DE=update_ptr)
      ex   de, hl                 ; DE = update_ptr
      pop  hl                     ; HL = pTChar. Stack: [retaddr(2), y, x, plane]
      push de                     ; save update_ptr. Stack: [update_ptr(2), retaddr(2), y, x, plane]
      EXTERN asm_sp1_InsertCharStruct
      call asm_sp1_InsertCharStruct
      ;
      ; asm_sp1_InvUpdateStruct(HL=update_ptr)
      pop  hl                     ; HL = update_ptr. Stack: [retaddr(2), y, x, plane]
      EXTERN asm_sp1_InvUpdateStruct
      call asm_sp1_InvUpdateStruct
      ;
      ; Callee cleanup: y(1) + x(1) + plane(1) = 3 bytes + return
      pop  hl                     ; HL = retaddr. Stack: [y, x, plane]
      inc  sp                     ; skip y
      inc  sp                     ; skip x
      inc  sp                     ; skip plane
      jp   (hl)                   ; return to caller
   __endasm;
}

// void sp1_IterateSprChar(struct sp1_ss *s, void *hook1)
// sdcccall(1): HL=s, DE=hook1
// asm: HL=s, IX=hook1
// asm calls hook via l_jpix (jp ix) with __SDCC push order:
//   push hl (&cs), push bc (count) -> hook sees [IX+4]=count, [IX+6]=&cs
void sp1_IterateSprChar(struct sp1_ss *s, void *hook1) __naked __sdcccall(1) {
   s; hook1;
   __asm
      EXTERN asm_sp1_IterateSprChar
      push ix
      push de
      pop  ix               ; IX = hook1
      call asm_sp1_IterateSprChar
      pop  ix               ; restore caller IX
      ret
   __endasm;
}

// void sp1_RemoveCharStruct(struct sp1_cs *cs)
// sdcc: HL=cs  asm: HL=cs
void sp1_RemoveCharStruct(struct sp1_cs *cs) __naked __sdcccall(1) {
   cs;
   __asm
      EXTERN asm_sp1_RemoveCharStruct
      jp   asm_sp1_RemoveCharStruct
   __endasm;
}

// void sp1_PutTiles(struct sp1_Rect *r, struct sp1_tp *src)
// sdcc: HL=r, DE=src
// asm_sp1_PutTiles: HL=src, D=row, E=col, B=width, C=height
void sp1_PutTiles(struct sp1_Rect *r, struct sp1_tp *src) __naked __sdcccall(1) {
   r; src;
   __asm
      EXTERN asm_sp1_PutTiles
      push de          ; save src
      ld   d, (hl)     ; D = row
      inc  hl
      ld   e, (hl)     ; E = col
      inc  hl
      ld   b, (hl)     ; B = width
      inc  hl
      ld   c, (hl)     ; C = height
      pop  hl          ; HL = src
      push ix          ; save IX: asm_sp1_PutTiles clobbers IXL (uses it as row counter)
      call asm_sp1_PutTiles
      pop  ix
      ret
   __endasm;
}

// void sp1_PutTilesInv(struct sp1_Rect *r, struct sp1_tp *src)
// sdcc: HL=r, DE=src
// asm_sp1_PutTilesInv: HL=src, D=row, E=col, B=width, C=height
void sp1_PutTilesInv(struct sp1_Rect *r, struct sp1_tp *src) __naked __sdcccall(1) {
   r; src;
   __asm
      EXTERN asm_sp1_PutTilesInv
      push de          ; save src
      ld   d, (hl)     ; D = row
      inc  hl
      ld   e, (hl)     ; E = col
      inc  hl
      ld   b, (hl)     ; B = width
      inc  hl
      ld   c, (hl)     ; C = height
      pop  hl          ; HL = src
      push ix          ; save IX: asm_sp1_PutTilesInv clobbers all of IX (loads update list tail ptr)
      call asm_sp1_PutTilesInv
      pop  ix
      ret
   __endasm;
}
