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
void sp1_UpdateNow(void) __naked __sdcccall(1) {
   __asm
      jp   asm_sp1_UpdateNow
   __endasm;
}

// void sp1_Initialize(u8 iflag, u8 colour, u8 tile)
// sdcc: A=iflag, L=colour, [SP+2]=tile
// asm:  A=iflag, H=colour, L=tile
void sp1_Initialize(uint8_t iflag, uint8_t colour, uint8_t tile) __naked __sdcccall(1) {
   iflag; colour; tile;
   __asm
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
struct sp1_update *sp1_GetUpdateStruct(uint8_t row, uint8_t col) __naked __sdcccall(1) {
   row; col;
   __asm
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
      jp   asm_sp1_InvUpdateStruct
   __endasm;
}

// void sp1_IterateUpdateArr(struct sp1_update **ua, void *hook)
// sdcc: HL=ua, DE=hook  asm: HL=array, IX=hook
void sp1_IterateUpdateArr(struct sp1_update **ua, void *hook) __naked __sdcccall(1) {
   ua; hook;
   __asm
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
      jp   asm_sp1_DeleteSpr
   __endasm;
}

// void sp1_MoveSprAbs(struct sp1_ss *s, struct sp1_Rect *clip, void *frame, u8 row, u8 col, u8 vrot, u8 hrot)
// sdcc ACTUAL: HL=s, DE=clip, A=row (from half-push trick, still in A at call)
//   Stack at entry: [ret(2), frame(2), row(1), col(1), vrot(1), hrot(1)]
//   (SDCC pushes frame to stack, not BC, for this 7-arg function)
// asm:  IX=s, IY=clip, HL=frame, D=row, E=col, B=hrot, C=vrot
void sp1_MoveSprAbs(struct sp1_ss *s, struct sp1_Rect *clip, void *frame,
                    uint8_t row, uint8_t col, uint8_t vrot, uint8_t hrot) __naked __sdcccall(1) {
   s; clip; frame; row; col; vrot; hrot;
   __asm
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

// void sp1_MoveSprRel(struct sp1_ss *s, struct sp1_Rect *clip, void *frame, i8 rel_row, i8 rel_col, i8 rel_vrot, i8 rel_hrot)
// sdcc ACTUAL: HL=s, DE=clip, A=rel_row
//   Stack: [ret(2), frame(2), rel_row(1), rel_col(1), rel_vrot(1), rel_hrot(1)]
//   rel_vrot/rel_hrot packed together by SDCC push de: E=rel_vrot at SP+6, D=rel_hrot at SP+7
// asm:  IX=s, IY=clip, HL=frame, D=rel_row, E=rel_col, B=rel_hrot, C=rel_vrot
void sp1_MoveSprRel(struct sp1_ss *s, struct sp1_Rect *clip, void *frame,
                    int8_t rel_row, int8_t rel_col, int8_t rel_vrot, int8_t rel_hrot) __naked __sdcccall(1) {
   s; clip; frame; rel_row; rel_col; rel_vrot; rel_hrot;
   __asm
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

// void sp1_MoveSprPix(struct sp1_ss *s, struct sp1_Rect *clip, void *frame, u16 x, u16 y)
// asm:  IX=s, IY=clip, HL=frame, DE=x, BC=y
// SDCC always pushes frame, x, y onto the stack (6 bytes callee-cleanup).
// BC may hold frame or x depending on register allocation, but is ignored.
// Stack layout at entry (both call sites): [ret | frame | x | y]
void sp1_MoveSprPix(struct sp1_ss *s, struct sp1_Rect *clip, void *frame,
                    uint16_t x, uint16_t y) __naked __sdcccall(1) {
   s; clip; frame; x; y;
   __asm
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
      ld   de, _SP1_DRAW_MASK2NR  ; DE = drawf
      ld   a, 0x40                ; A = SP1_TYPE_2BYTE (must reload: was clobbered by HL construction)
      call asm_sp1_InitCharStruct
      ;
      ; asm_sp1_GetUpdateStruct(D=y, E=x) -> HL=update_ptr
      pop  de                     ; pop de: E=x(SP+0), D=y(SP+1). Stack: [pTChar(2), retaddr(2), y,x,plane]
      call asm_sp1_GetUpdateStruct ; HL = update_ptr
      ;
      ; asm_sp1_InsertCharStruct(HL=cs, DE=update_ptr)
      ex   de, hl                 ; DE = update_ptr
      pop  hl                     ; HL = pTChar. Stack: [retaddr(2), y, x, plane]
      push de                     ; save update_ptr. Stack: [update_ptr(2), retaddr(2), y, x, plane]
      call asm_sp1_InsertCharStruct
      ;
      ; asm_sp1_InvUpdateStruct(HL=update_ptr)
      pop  hl                     ; HL = update_ptr. Stack: [retaddr(2), y, x, plane]
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
// asm: HL=s, IX=hook1
// asm calls hook via l_jpix (jp ix) with __SDCC push order:
//   push hl (&cs), push bc (count) -> hook sees [IX+4]=count, [IX+6]=&cs
void sp1_IterateSprChar(struct sp1_ss *s, void *hook1) __naked __sdcccall(1) {
   s; hook1;
   __asm
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
      jp   asm_sp1_RemoveCharStruct
   __endasm;
}

// void sp1_PutTiles(struct sp1_Rect *r, struct sp1_tp *src)
// sdcc: HL=r, DE=src
// asm_sp1_PutTiles: HL=src, D=row, E=col, B=width, C=height
void sp1_PutTiles(struct sp1_Rect *r, struct sp1_tp *src) __naked __sdcccall(1) {
   r; src;
   __asm
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




// =============================================================================
// SP1 ENGINE — ALL MODULES EMBEDDED
// All 31 ASM modules merged into one inline block.
// EXTERN declarations for SP1-internal symbols removed (resolved locally).
// Only __malloc / __free remain EXTERN (defined in crt0.asm).
// =============================================================================
static void __sp1_asm_engine(void) __naked {
  __asm

    ; =========================================================================
    ; SECTION + CONFIG (expanded from sp1___config.asm)
    ; =========================================================================
    SECTION _CODE_SP1

	; display characteristics
    defc SP1V_DISPORIGX   = 0		; x coordinate of top left corner of area managed by sp1 in characters
    defc SP1V_DISPORIGY   = 0		; y coordinate of top left corner of area managed by sp1 in characters
    defc SP1V_DISPWIDTH   = 32		; width of area managed by sp1 in characters (16, 24, 32 ok as of now)
    defc SP1V_DISPHEIGHT  = 24		; height of area managed by sp1 in characters
	
	; sp1 variables
    defc SP1V_UPDATELISTH = 0xD9ED	; address of 10-byte area holding a dummy struct_sp1_update that is always the "first" in list of screen tiles to be drawn
    defc SP1V_UPDATELISTT = 0xD9EF	; address of 2-byte variable holding the address of the last struct_sp1_update in list of screen tiles to be drawn
									; note: SP1V_UPDATELISTT is located inside the dummy struct_sp1_update pointed at by SP1V_UPDATELISTH
	; buffers
    defc SP1V_PIXELBUFFER = 0xD9F7	; address of an 8-byte buffer to hold intermediate pixel-draw results
    defc SP1V_ATTRBUFFER  = 0xD9FF	; address of a single byte buffer to hold intermediate colour-draw results
    
	; data structure locations
	defc SP1V_UPDATEARRAY = 0xDA00	; address of the 10*SP1V_DISPWIDTH*SP1V_DISPHEIGHT byte update array
    defc SP1V_TILEARRAY   = 0xF800	; address of the 512-byte tile array associating character codes with tile graphics, must lie on 256-byte boundary (LSB=0)
    defc SP1V_ROTTBL      = 0xF800	; location of the 3584-byte rotation table.  Must lie on 256-byte boundary (LSB=0).  Table begins $0200 bytes ahead of this
                                    ;  pointer ($f200-$ffff in this default case). Set to $0000 if the table is not needed (if, for example, all sprites are
                                    ;  drawn at exact horizontal character coordinates or you use pre-shifted sprites only)
    
	EXTERN __malloc
    EXTERN __free

    ; =========================================================================
    ; MODULE: sp1__struct_ss_prototype
    ; =========================================================================
    PUBLIC _sp1_struct_ss_prototype

    _sp1_struct_ss_prototype:

       defb 0
       defb SP1V_DISPWIDTH
       defb 1
       defb 0
       defb 0
       defb 0
       defw 0

       ld a,SP1V_ROTTBL/256
       ld bc,0
       ex de,hl
       jp (hl)

       defw 0

       defb 1
       defb 1
       defb 0

    ; =========================================================================
    ; MODULE: sp1__struct_cs_prototype
    ; =========================================================================
    PUBLIC _sp1_struct_cs_prototype

    _sp1_struct_cs_prototype:

       defw 0
       defw 0
       defb 0
       defb 0
       defb $ff
       defb 0
       defw 0

       ld hl,0
       ld ix,0
       call 0

       defw 0
       defw 0

    ; =========================================================================
    ; MODULE: sp1_DRAW_MASK2NR
    ; =========================================================================
    PUBLIC _SP1_DRAW_MASK2NR

       ld hl,0
       nop
       ld de,0
       call _SP1_DRAW_MASK2NR

    _SP1_DRAW_MASK2NR:

       add hl,bc

       ld de,(SP1V_PIXELBUFFER+0)
       ld a,(hl)
       and e
       inc hl
       or (hl)
       inc hl
       ld (SP1V_PIXELBUFFER+0),a
       ld a,(hl)
       and d
       inc hl
       or (hl)
       inc hl
       ld (SP1V_PIXELBUFFER+1),a

       ld de,(SP1V_PIXELBUFFER+2)
       ld a,(hl)
       and e
       inc hl
       or (hl)
       inc hl
       ld (SP1V_PIXELBUFFER+2),a
       ld a,(hl)
       and d
       inc hl
       or (hl)
       inc hl
       ld (SP1V_PIXELBUFFER+3),a

       ld de,(SP1V_PIXELBUFFER+4)
       ld a,(hl)
       and e
       inc hl
       or (hl)
       inc hl
       ld (SP1V_PIXELBUFFER+4),a
       ld a,(hl)
       and d
       inc hl
       or (hl)
       inc hl
       ld (SP1V_PIXELBUFFER+5),a

       ld de,(SP1V_PIXELBUFFER+6)
       ld a,(hl)
       and e
       inc hl
       or (hl)
       inc hl
       ld (SP1V_PIXELBUFFER+6),a
       ld a,(hl)
       and d
       inc hl
       or (hl)
       ld (SP1V_PIXELBUFFER+7),a

       jp SP1RETSPRDRAW

    ; =========================================================================
    ; MODULE: sp1_DRAW_MASK2LB
    ; =========================================================================
    PUBLIC _SP1_DRAW_MASK2LB

       ld hl,0
       nop
       ld de,0
       call _SP1_DRAW_MASK2LB

    _SP1_DRAW_MASK2LB:

       cp SP1V_ROTTBL/256
       jp z, _SP1_DRAW_MASK2NR

       add hl,bc
       ld d,a

       ld e,$ff
       ld a,(de)
       cpl
       exx
       ld b,a
       exx

    _SP1Mask2LBRotate:

       ld bc,(SP1V_PIXELBUFFER+0)
       ld e,(hl)
       inc hl
       ld a,(de)
       exx
       or b
       exx
       and c
       ld c,a
       ld e,(hl)
       inc hl
       ld a,(de)
       or c
       ld (SP1V_PIXELBUFFER+0),a
       ld e,(hl)
       inc hl
       ld a,(de)
       exx
       or b
       exx
       and b
       ld b,a
       ld e,(hl)
       inc hl
       ld a,(de)
       or b
       ld (SP1V_PIXELBUFFER+1),a

       ld bc,(SP1V_PIXELBUFFER+2)
       ld e,(hl)
       inc hl
       ld a,(de)
       exx
       or b
       exx
       and c
       ld c,a
       ld e,(hl)
       inc hl
       ld a,(de)
       or c
       ld (SP1V_PIXELBUFFER+2),a
       ld e,(hl)
       inc hl
       ld a,(de)
       exx
       or b
       exx
       and b
       ld b,a
       ld e,(hl)
       inc hl
       ld a,(de)
       or b
       ld (SP1V_PIXELBUFFER+3),a

       ld bc,(SP1V_PIXELBUFFER+4)
       ld e,(hl)
       inc hl
       ld a,(de)
       exx
       or b
       exx
       and c
       ld c,a
       ld e,(hl)
       inc hl
       ld a,(de)
       or c
       ld (SP1V_PIXELBUFFER+4),a
       ld e,(hl)
       inc hl
       ld a,(de)
       exx
       or b
       exx
       and b
       ld b,a
       ld e,(hl)
       inc hl
       ld a,(de)
       or b
       ld (SP1V_PIXELBUFFER+5),a

       ld bc,(SP1V_PIXELBUFFER+6)
       ld e,(hl)
       inc hl
       ld a,(de)
       exx
       or b
       exx
       and c
       ld c,a
       ld e,(hl)
       inc hl
       ld a,(de)
       or c
       ld (SP1V_PIXELBUFFER+6),a
       ld e,(hl)
       inc hl
       ld a,(de)
       exx
       or b
       exx
       and b
       ld b,a
       ld e,(hl)
       ld a,(de)
       or b
       ld (SP1V_PIXELBUFFER+7),a

       jp SP1RETSPRDRAW

    ; =========================================================================
    ; MODULE: sp1_DRAW_MASK2RB
    ; =========================================================================
    PUBLIC _SP1_DRAW_MASK2RB

       ld de,0
       nop
       ld hl,0
       call _SP1_DRAW_MASK2RB

    _SP1_DRAW_MASK2RB:

       cp SP1V_ROTTBL/256
       jp z, SP1RETSPRDRAW

       add hl,bc
       ld d,a
       inc d

    _SP1Mask2RBRotate:

       jp _SP1_DRAW_MASK2LB + 7

    ; =========================================================================
    ; MODULE: sp1_DRAW_MASK2
    ; =========================================================================
    PUBLIC _SP1_DRAW_MASK2

       ld hl,0
       ld ix,0
       call _SP1_DRAW_MASK2

    _SP1_DRAW_MASK2:

       cp SP1V_ROTTBL/256
       jp z, _SP1_DRAW_MASK2NR

       add hl,bc
       add ix,bc
       ex de,hl
       ld h,a

    _SP1Mask2Rotate:

       ld a,(de)
       inc de
       ld l,a
       ld a,(hl)
       inc h
       ld l,(ix+0)
       or (hl)
       ld b,a
       ld l,(ix+1)
       ld c,(hl)
       dec h
       ld a,(de)
       inc de
       ld l,a
       ld a,(SP1V_PIXELBUFFER+0)
       and b
       or c
       or (hl)
       ld (SP1V_PIXELBUFFER+0),a

       ld a,(de)
       inc de
       ld l,a
       ld a,(hl)
       inc h
       ld l,(ix+2)
       or (hl)
       ld b,a
       ld l,(ix+3)
       ld c,(hl)
       dec h
       ld a,(de)
       inc de
       ld l,a
       ld a,(SP1V_PIXELBUFFER+1)
       and b
       or c
       or (hl)
       ld (SP1V_PIXELBUFFER+1),a

       ld a,(de)
       inc de
       ld l,a
       ld a,(hl)
       inc h
       ld l,(ix+4)
       or (hl)
       ld b,a
       ld l,(ix+5)
       ld c,(hl)
       dec h
       ld a,(de)
       inc de
       ld l,a
       ld a,(SP1V_PIXELBUFFER+2)
       and b
       or c
       or (hl)
       ld (SP1V_PIXELBUFFER+2),a

       ld a,(de)
       inc de
       ld l,a
       ld a,(hl)
       inc h
       ld l,(ix+6)
       or (hl)
       ld b,a
       ld l,(ix+7)
       ld c,(hl)
       dec h
       ld a,(de)
       inc de
       ld l,a
       ld a,(SP1V_PIXELBUFFER+3)
       and b
       or c
       or (hl)
       ld (SP1V_PIXELBUFFER+3),a

       ld a,(de)
       inc de
       ld l,a
       ld a,(hl)
       inc h
       ld l,(ix+8)
       or (hl)
       ld b,a
       ld l,(ix+9)
       ld c,(hl)
       dec h
       ld a,(de)
       inc de
       ld l,a
       ld a,(SP1V_PIXELBUFFER+4)
       and b
       or c
       or (hl)
       ld (SP1V_PIXELBUFFER+4),a

       ld a,(de)
       inc de
       ld l,a
       ld a,(hl)
       inc h
       ld l,(ix+10)
       or (hl)
       ld b,a
       ld l,(ix+11)
       ld c,(hl)
       dec h
       ld a,(de)
       inc de
       ld l,a
       ld a,(SP1V_PIXELBUFFER+5)
       and b
       or c
       or (hl)
       ld (SP1V_PIXELBUFFER+5),a

       ld a,(de)
       inc de
       ld l,a
       ld a,(hl)
       inc h
       ld l,(ix+12)
       or (hl)
       ld b,a
       ld l,(ix+13)
       ld c,(hl)
       dec h
       ld a,(de)
       inc de
       ld l,a
       ld a,(SP1V_PIXELBUFFER+6)
       and b
       or c
       or (hl)
       ld (SP1V_PIXELBUFFER+6),a

       ld a,(de)
       inc de
       ld l,a
       ld a,(hl)
       inc h
       ld l,(ix+14)
       or (hl)
       ld b,a
       ld l,(ix+15)
       ld c,(hl)
       dec h
       ld a,(de)
       ld l,a
       ld a,(SP1V_PIXELBUFFER+7)
       and b
       or c
       or (hl)
       ld (SP1V_PIXELBUFFER+7),a

       jp SP1RETSPRDRAW

    ; =========================================================================
    ; MODULE: sp1_DrawUpdateStruct
    ; =========================================================================
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

    ; =========================================================================
    ; MODULE: sp1_GetUpdateStruct
    ; =========================================================================
    PUBLIC asm_sp1_GetUpdateStruct

    asm_sp1_GetUpdateStruct:

       ld l,d
       ld h,0
       ld a,d
       ld d,h
       cp SP1V_DISPHEIGHT
       jp c, nohtadj
       dec h

    .nohtadj

       IF SP1V_DISPWIDTH=32

          add hl,hl
          add hl,hl
          add hl,hl
          add hl,hl
          add hl,hl
          ld a,e
          cp SP1V_DISPWIDTH
          jp c, nowiadj
          dec d

       .nowiadj

          add hl,de

       ENDIF

       add hl,hl
       ld d,h
       ld e,l
       add hl,hl
       add hl,hl
       add hl,de

       ld de,SP1V_UPDATEARRAY
       add hl,de

       ret

    ; =========================================================================
    ; MODULE: sp1_InvUpdateStruct
    ; =========================================================================
    PUBLIC asm_sp1_InvUpdateStruct

    asm_sp1_InvUpdateStruct:

       ld a,$80
       xor (hl)
       ret p
       ld (hl),a

       ex de,hl
       ld hl,(SP1V_UPDATELISTT)
       ld bc,6
       add hl,bc
       ld (hl),d
       inc hl
       ld (hl),e
       ex de,hl
       ld (SP1V_UPDATELISTT),hl
       add hl,bc
       ld (hl),0

       ret

    ; =========================================================================
    ; MODULE: sp1_UpdateNow
    ; =========================================================================
    PUBLIC asm_sp1_UpdateNow

    asm_sp1_UpdateNow:

       push ix
       push iy

       ld hl,(SP1V_UPDATELISTH+6)
       ld a,l
       ld l,h
       ld h,a
       or a
       jp nz, UpdateNow_updatelp

       jr UpdateNow_all_return

    .UpdateNow_skipthischar

       ld bc,6
       add hl,bc
       ld a,(hl)
       or a
       jr z, UpdateNow_doneupdate
       inc hl
       ld l,(hl)
       ld h,a

    .UpdateNow_updatelp

       bit 6,(hl)
       jr nz, UpdateNow_skipthischar

       ld a,$80
       xor (hl)
       jp m, UpdateNow_skipthischar
       ld (hl),a

       ld b,a

       call SP1DrawUpdateStruct

       ld l,c
       ld h,b

       inc b
       djnz UpdateNow_updatelp

    .UpdateNow_doneupdate

       xor a
       ld (SP1V_UPDATELISTH+6),a
       ld hl,SP1V_UPDATELISTH
       ld (SP1V_UPDATELISTT),hl

    .UpdateNow_all_return:
       pop iy
       pop ix
       ret

    ; =========================================================================
    ; MODULE: sp1_Invalidate
    ; =========================================================================
    PUBLIC asm_sp1_Invalidate

    asm_sp1_Invalidate:

       call asm_sp1_GetUpdateStruct
       ex de,hl
       ld hl,(SP1V_UPDATELISTT)
       ld a,6
       add a,l
       ld l,a
       jp nc, rowlp
       inc h

    .rowlp

       push bc
       push de

    .collp

       ld a,(de)
       xor $80
       jp p, alreadyinlist

       ld (de),a
       ld (hl),d
       inc hl
       ld (hl),e

       ld hl,6
       add hl,de

    .alreadyinlist

       ld a,10
       add a,e
       ld e,a
       jp nc, noinc
       inc d

    .noinc

       djnz collp

       pop de
       ex de,hl
       ld bc,10*SP1V_DISPWIDTH
       add hl,bc
       ex de,hl
       pop bc

       dec c
       jp nz, rowlp

       ld (hl),0
       ld bc,-6
       add hl,bc
       ld (SP1V_UPDATELISTT),hl

       ret

    ; =========================================================================
    ; MODULE: sp1_ClearRect  (also defines SP1CRSELECT and l_jpix)
    ; =========================================================================
    PUBLIC asm_sp1_ClearRect
    PUBLIC SP1CRSELECT
    PUBLIC l_jpix

    asm_sp1_ClearRect:

       and $07
       ret z

       push hl
       call SP1CRSELECT
       call asm_sp1_GetUpdateStruct
       pop de

    .ClearRect_rowloop

       push bc
       push hl

    .ClearRect_colloop

       call l_jpix
       djnz ClearRect_colloop

       pop hl
       ld bc,10*SP1V_DISPWIDTH
       add hl,bc
       pop bc

       dec c
       jp nz, ClearRect_rowloop

       ret

    SP1CRSELECT:

       push de
       add a,a
       ld e,a
       ld d,0
       ld hl,ClearRect_seltbl
       add hl,de
       ld a,(hl)
       ld ixl,a
       inc hl
       ld a,(hl)
       ld ixh,a
       pop de

       ret

    .ClearRect_seltbl

       defw ClearRect_OPTION0, ClearRect_OPTION1, ClearRect_OPTION2, ClearRect_OPTION3
       defw ClearRect_OPTION4, ClearRect_OPTION5, ClearRect_OPTION6, ClearRect_OPTION7

    .ClearRect_OPTION0:

       ld a,10
       add a,l
       ld l,a
       ret nc
       inc h
       ret

    .ClearRect_OPTION1:

       inc hl
       inc hl
       ld (hl),e
       inc hl
       ld (hl),0
       ld a,7
       add a,l
       ld l,a
       ret nc
       inc h
       ret

    .ClearRect_OPTION2:

       inc hl
       ld (hl),d
       ld a,9
       add a,l
       ld l,a
       ret nc
       inc h
       ret

    .ClearRect_OPTION3:

       inc hl
       ld (hl),d
       inc hl
       ld (hl),e
       inc hl
       ld (hl),0
       ld a,7
       add a,l
       ld l,a
       ret nc
       inc h
       ret

    .ClearRect_OPTION4:

       ld a,(hl)
       and $c0
       inc a
       ld (hl),a
       inc hl
       inc hl
       inc hl
       inc hl
       push hl

       ld a,(hl)
       or a
       jr z, ClearRect_done

       ld (hl),0
       inc hl
       ld l,(hl)
       ld h,a

    .ClearRect_loop

       dec hl
       dec hl
       dec hl
       dec hl

       ld (hl),0
       ld a,18
       add a,l
       ld l,a
       jp nc, ClearRect_noinc1
       inc h

    .ClearRect_noinc1

       ld a,(hl)
       or a
       jr z, ClearRect_done

       inc hl
       ld l,(hl)
       ld h,a
       jp ClearRect_loop

    .ClearRect_done

       pop hl
       ld a,6
       add a,l
       ld l,a
       ret nc
       inc h
       ret

    .ClearRect_OPTION5:

       inc hl
       inc hl
       ld (hl),e
       inc hl
       ld (hl),0
       dec hl
       dec hl
       dec hl
       jp ClearRect_OPTION4

    .ClearRect_OPTION6:

       inc hl
       ld (hl),d
       dec hl
       jp ClearRect_OPTION4

    .ClearRect_OPTION7:

       inc hl
       ld (hl),d
       inc hl
       ld (hl),e
       inc hl
       ld (hl),0
       dec hl
       dec hl
       dec hl
       jp ClearRect_OPTION4

    l_jpix:
       jp (ix)

    ; =========================================================================
    ; MODULE: sp1_ClearRectInv
    ; =========================================================================
    PUBLIC asm_sp1_ClearRectInv

    asm_sp1_ClearRectInv:

       and $07
       ret z

       push hl
       call SP1CRSELECT
       call asm_sp1_GetUpdateStruct
       pop de

       ld iy,(SP1V_UPDATELISTT)

    .rowloop

       push bc
       push hl

    .colloop

       ld a,$80
       xor (hl)
       jp p, alreadyinv
       ld (hl),a

       ld (iy+6),h
       ld (iy+7),l

       ld a,l
       ld iyl,a
       ld a,h
       ld iyh,a

    .alreadyinv

       call l_jpix
       djnz colloop

       pop hl
       ld bc,10*SP1V_DISPWIDTH
       add hl,bc
       pop bc

       dec c
       jp nz, rowloop

       ld (iy+6),0
       ld (SP1V_UPDATELISTT),iy
       ret

    ; =========================================================================
    ; MODULE: sp1__add_spr_char
    ; =========================================================================
    PUBLIC __sp1_add_spr_char

    __sp1_add_spr_char:

       ld d,(hl)
       inc hl
       inc d
       dec d
       jr z, _add_spr_char_donesearch1

       ld e,(hl)
       dec de
       dec de
       ex de,hl
       cp (hl)
       jr nc, _add_spr_char_donesearch0
       ld de,16
       add hl,de

       jp __sp1_add_spr_char

    ._add_spr_char_donesearch1:

       ld (hl),c
       dec hl
       ld (hl),b
       ex de,hl
       ld hl,14
       add hl,bc
       ld (hl),0
       inc hl
       inc hl
       ld (hl),d
       inc hl
       ld (hl),e

       ret

    ._add_spr_char_donesearch0:

       inc hl
       inc hl
       ex de,hl
       ld (hl),c
       dec hl
       ld (hl),b
       push hl
       ld hl,14
       add hl,bc
       ld (hl),d
       inc hl
       ld (hl),e
       inc hl
       pop bc
       ld (hl),b
       inc hl
       ld (hl),c
       dec hl
       dec hl
       dec hl
       ex de,hl
       ld bc,16
       add hl,bc
       ld (hl),d
       inc hl
       ld (hl),e

       ret

    ; =========================================================================
    ; MODULE: sp1__remove_spr_char
    ; =========================================================================
    PUBLIC __sp1_remove_spr_char

    __sp1_remove_spr_char:

       ld de,14
       add hl,de

       ld b,(hl)
       inc b
       inc hl
       djnz _remove_spr_char_nextexists

       inc hl
       ld d,(hl)
       inc hl
       ld e,(hl)
       ex de,hl
       ld (hl),0

       ret

    ._remove_spr_char_nextexists:

       ld c,(hl)
       inc hl
       ld d,(hl)
       inc hl
       ld e,(hl)

       ex de,hl
       push hl

       ld (hl),b
       inc hl
       ld (hl),c

       ld hl,16
       add hl,bc
       pop bc

       ld (hl),b
       inc hl
       ld (hl),c

       ret

    ; =========================================================================
    ; MODULE: sp1_InitCharStruct
    ; =========================================================================
    PUBLIC asm_sp1_InitCharStruct

    asm_sp1_InitCharStruct:

       push bc
       push de

       ex de,hl
       ld hl,_sp1_struct_cs_prototype
       ld bc,24
       ldir

       ld hl,-5
       add hl,de
       pop de
       dec de
       ex de,hl

       ldd
       ldd
       ldd
       dec hl
       dec hl
       dec de
       dec de
       ldd
       ldd
       pop bc
       ex de,hl
       ld (hl),b
       dec hl
       ld (hl),c
       dec hl
       dec de
       dec de
       ex de,hl
       ldd

       ex de,hl
       ld de,InitCharStruct_sp1_ss_embedded
       ld (hl),d
       dec hl
       ld (hl),e

       dec hl
       dec hl
       dec hl
       ld (hl),a
       dec hl
       ex af,af
       ld (hl),a

       ret

    .InitCharStruct_sp1_ss_embedded:

       ld a,SP1V_ROTTBL/256 + 4
       ld bc,0
       ex de,hl
       jp (hl)

    ; =========================================================================
    ; MODULE: sp1_InsertCharStruct
    ; =========================================================================
    PUBLIC asm_sp1_InsertCharStruct

    asm_sp1_InsertCharStruct:

       inc hl
       inc hl
       ld (hl),d
       inc hl
       ld (hl),e
       inc hl
       ld a,(hl)
       inc hl

       bit 7,(hl)
       ex de,hl
       jr z, InsertCharStruct_notoccluding
       inc (hl)

    .InsertCharStruct_notoccluding:

       inc de
       ld c,e
       ld b,d
       ld de,4
       add hl,de
       jp __sp1_add_spr_char

    ; =========================================================================
    ; MODULE: sp1_RemoveCharStruct
    ; =========================================================================
    PUBLIC asm_sp1_RemoveCharStruct

    asm_sp1_RemoveCharStruct:

       inc hl
       inc hl

       ld a,(hl)
       or a
       ret z

       ld d,a
       ld (hl),0
       inc hl
       ld e,(hl)
       inc hl
       inc hl

       bit 7,(hl)
       ex de,hl
       jr z, RemoveCharStruct_notoccluding
       dec (hl)

    .RemoveCharStruct_notoccluding:

       inc de
       ld hl,17
       add hl,de
       ex de,hl
       jp __sp1_remove_spr_char

    ; =========================================================================
    ; MODULE: sp1_CreateSpr
    ; =========================================================================
    PUBLIC asm_sp1_CreateSpr

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
       call __malloc
       pop bc
       jp c, CreateSpr_fail

       pop bc
       push hl
       djnz CreateSpr_csalloc

       ld hl,20
       push hl
       call __malloc
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
       call __free

       pop hl
       jp CreateSpr_faillp

    .CreateSpr_all_return:
       pop iy
       pop ix
       ret

    ; =========================================================================
    ; MODULE: sp1_AddColSpr
    ; =========================================================================
    PUBLIC asm_sp1_AddColSpr

    asm_sp1_AddColSpr:

       exx
       ld hl,0
       push hl
       ld b,(ix+3)

    .AddColSpr_csalloc:

       push bc
       ld hl,24
       push hl
       call __malloc
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

       ld (iy+4),h
       ld a,l
       and $90
       or $40
       ld (iy+5),a

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
       call __free

       pop hl
       jr AddColSpr_faillp

    ; =========================================================================
    ; MODULE: sp1_DeleteSpr
    ; =========================================================================
    PUBLIC asm_sp1_DeleteSpr

    asm_sp1_DeleteSpr:

       ex de,hl
       ld hl,15
       add hl,de

    .DeleteSpr_loop:

       ld b,(hl)
       inc hl
       ld c,(hl)
       push bc
       ex de,hl
       push hl
       call __free

       pop hl
       pop de
       ld l,e
       ld h,d

       inc h
       dec h
       jp nz, DeleteSpr_loop

       ret

    ; =========================================================================
    ; MODULE: sp1_MoveSprAbs
    ; =========================================================================
    PUBLIC asm_sp1_MoveSprAbs

    asm_sp1_MoveSprAbs:

       ld (ix+5),b
       ld a,b

       cp (ix+17)
       rl b

       or  SP1V_ROTTBL/256
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

    ; =========================================================================
    ; MODULE: sp1_MoveSprPix
    ; =========================================================================
    PUBLIC asm_sp1_MoveSprPix

    asm_sp1_MoveSprPix:

       ld a,e
       and $07
       srl d
       rr e
       srl d
       rr e
       srl d
       rr e
       ld d,b
       ld b,a

       ld a,c
       and $07
       srl d
       rr c
       srl d
       rr c
       srl d
       rr c
       ld d,c
       ld c,a

       jp asm_sp1_MoveSprAbs

    ; =========================================================================
    ; MODULE: sp1_MoveSprRel  (note: original uses bare SECTION, stripped here)
    ; =========================================================================
    PUBLIC asm_sp1_MoveSprRel

    asm_sp1_MoveSprRel:

       ld a,(ix+5)
       add a,b
       ld b,a
       sra a
       sra a
       sra a
       add a,e
       add a,(ix+1)
       ld e,a
       ld a,b
       cp 0x80
       jp c, mvpos1
       add a,8

    .mvpos1

       and 0x07
       ld b,a
       ld a,(ix+4)
       and 0x07
       add a,c
       ld c,a
       sra a
       sra a
       sra a
       add a,d
       add a,(ix+0)
       ld d,a
       ld a,c
       cp 0x80
       jp c, mvpos2
       add a,8

    .mvpos2

       and 0x07
       ld c,a

       jp asm_sp1_MoveSprAbs

    ; =========================================================================
    ; MODULE: sp1_IterateSprChar
    ; =========================================================================
    PUBLIC asm_sp1_IterateSprChar

    asm_sp1_IterateSprChar:

       ld bc,15
       add hl,bc

       ld c,b

    .IterateSprChar_iterloop:

       ld a,(hl)
       or a
       ret z

       inc hl
       ld l,(hl)
       ld h,a

       push ix

       push hl
       push bc
       ld   d, h
       ld   e, l
       ld   l, c
       ld   h, b

       call l_jpix

       pop bc
       pop hl

       pop ix

       inc bc
       jp IterateSprChar_iterloop

    ; =========================================================================
    ; MODULE: sp1_IterateUpdateArr
    ; =========================================================================
    PUBLIC asm_sp1_IterateUpdateArr

    asm_sp1_IterateUpdateArr:

    .IterateUpdateArr_loop:

       ld e,(hl)
       inc hl
       ld d,(hl)

       ld a,d
       or e
       ret z

       inc hl
       push ix
       push hl
       push de
       ex de,hl
       call l_jpix
       pop de
       pop hl
       pop ix

       jp IterateUpdateArr_loop

    ; =========================================================================
    ; MODULE: sp1_TileEntry
    ; =========================================================================
    PUBLIC asm_sp1_TileEntry

    asm_sp1_TileEntry:

       ld hl,SP1V_TILEARRAY
       ld b,0
       add hl,bc
       ld a,(hl)
       ld (hl),e
       ld e,a
       inc h
       ld a,(hl)
       ld (hl),d
       ld h,a
       ld l,e

       ret

    ; =========================================================================
    ; MODULE: sp1_PrintAt
    ; =========================================================================
    PUBLIC asm_sp1_PrintAt

    asm_sp1_PrintAt:

       ex af,af
       call asm_sp1_GetUpdateStruct
       ex af,af
       inc hl
       ld (hl),a
       inc hl
       ld (hl),c
       inc hl
       ld (hl),b
       ret

    ; =========================================================================
    ; MODULE: sp1_PutTiles  (note: original has bare SECTION, stripped here)
    ; =========================================================================
    PUBLIC asm_sp1_PutTiles

    asm_sp1_PutTiles:

       push ix
       push hl
       call asm_sp1_GetUpdateStruct
       pop de
       inc hl
       ex de,hl

       ld ixl,c
       ld c,$ff

    .PutTiles_rowloop

       push bc
       push de

    .PutTiles_colloop

       ldi
       ldi
       ldi
       ld a,7
       add a,e
       ld e,a
       jp nc, PutTiles_noinc
       inc d

    .PutTiles_noinc

       djnz PutTiles_colloop

       ex (sp),hl
       ld bc,10*SP1V_DISPWIDTH
       add hl,bc
       pop de
       ex de,hl

       pop bc

       dec ixl
       jp nz, PutTiles_rowloop

       pop ix
       ret

    ; =========================================================================
    ; MODULE: sp1_PutTilesInv  (note: original has bare SECTION, stripped here)
    ; =========================================================================
    PUBLIC asm_sp1_PutTilesInv

    asm_sp1_PutTilesInv:

       push ix
       push hl
       call asm_sp1_GetUpdateStruct
       pop de
       ex de,hl

       ld a,c
       ld c,$ff

       ld ix,(SP1V_UPDATELISTT)

    .PutTilesInv_rowloop

       push bc
       push de
       ex af,af

    .PutTilesInv_colloop

       ld a,(de)
       xor $80
       jp p, PutTilesInv_skipinval
       ld (de),a

       ld (ix+6),d
       ld (ix+7),e
       push de
       pop ix

    .PutTilesInv_skipinval

       inc de
       ldi
       ldi
       ldi
       ld a,6
       add a,e
       ld e,a
       jp nc, PutTilesInv_noinc
       inc d

    .PutTilesInv_noinc

       djnz PutTilesInv_colloop

       ex (sp),hl
       ld bc,10*SP1V_DISPWIDTH
       add hl,bc
       pop de
       ex de,hl

       pop bc

       ex af,af
       dec a
       jp nz, PutTilesInv_rowloop

       ld (ix+6),0
       ld (SP1V_UPDATELISTT),ix

       pop ix
       ret

    ; =========================================================================
    ; MODULE: sp1_Initialize
    ; =========================================================================
    PUBLIC asm_sp1_Initialize

    asm_sp1_Initialize:

       push hl

       bit 0,a
       jr z, norottbl

       ld c,6
       push af

    .rottbllp

       ld a,c
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

    .Initialize_rowloop

       ld c,SP1V_DISPORIGX

    .Initialize_colloop

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
       jr z, Initialize_skipscrnaddr
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

    .Initialize_rejoinscrnaddr

       inc hl
       inc c
       ld a,c
       cp SP1V_DISPORIGX + SP1V_DISPWIDTH
       jr c, Initialize_colloop

       inc b
       ld a,b
       cp SP1V_DISPORIGY + SP1V_DISPHEIGHT
       jr c, Initialize_rowloop

       ret

    .Initialize_skipscrnaddr

       ex af,af
       inc hl
       jp Initialize_rejoinscrnaddr



    ; =========================================================================
    SECTION _CODE      ; restore default section
    ; =========================================================================

  __endasm;
}
