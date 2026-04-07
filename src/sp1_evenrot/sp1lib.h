#ifndef _SP1LIB_H
#define _SP1LIB_H

#include <stdint.h>

///////////////////////////////////////////////////////////
//                  SPRITE PACK v3.0                     //
//             Sinclair Spectrum Version                 //
//            aralbrec - April / May 2006                //
///////////////////////////////////////////////////////////

#define INK_BLACK               0x00
#define INK_BLUE                0x01
#define INK_RED                 0x02
#define INK_MAGENTA             0x03
#define INK_GREEN               0x04
#define INK_CYAN                0x05
#define INK_YELLOW              0x06
#define INK_WHITE               0x07
#define PAPER_BLACK             0x00
#define PAPER_BLUE              0x08
#define PAPER_RED               0x10
#define PAPER_MAGENTA           0x18
#define PAPER_GREEN             0x20
#define PAPER_CYAN              0x28
#define PAPER_YELLOW            0x30
#define PAPER_WHITE             0x38

///////////////////////////////////////////////////////////
//                  DATA STRUCTURES                      //
///////////////////////////////////////////////////////////

struct sp1_Rect {

   uint8_t  row;
   uint8_t  col;
   uint8_t  width;
   uint8_t  height;

};

struct sp1_update;
struct sp1_ss;
struct sp1_cs;

struct sp1_update {                   // "update structs" - 10 bytes

   uint8_t           nload;          // +0 bit 7=invalidated, bit 6=removed, bits 5:0=occluding+1
   uint8_t           colour;         // +1 background tile attribute
   uint16_t          tile;           // +2 background tile code
   struct sp1_cs    *slist;          // +4 BIG ENDIAN
   struct sp1_update*ulist;          // +6 BIG ENDIAN
   uint8_t          *screen;         // +8

};

struct sp1_ss {                       // "sprite structs" - 20 bytes

   uint8_t           row;            // +0
   uint8_t           col;            // +1
   uint8_t           width;          // +2
   uint8_t           height;         // +3

   uint8_t           vrot;           // +4 bit7=2-byte def, bits2:0=vrot
   uint8_t           hrot;           // +5

   uint8_t          *frame;          // +6

   uint8_t           res0;           // +8
   uint8_t           e_hrot;         // +9
   uint8_t           res1;           // +10
   uint16_t          e_offset;       // +11
   uint8_t           res2;           // +13
   uint8_t           res3;           // +14

   struct sp1_cs    *first;          // +15 BIG ENDIAN

   uint8_t           xthresh;        // +17
   uint8_t           ythresh;        // +18
   uint8_t           nactive;        // +19

};

struct sp1_cs {                       // "char structs" - 24 bytes

   struct sp1_cs    *next_in_spr;    // +0  BIG ENDIAN
   struct sp1_update*update;         // +2  BIG ENDIAN
   uint8_t           plane;          // +4
   uint8_t           type;           // +5 bit7=occluding, bit6=last col, bit5=last row, bit4=clear pixelbuf
   uint8_t           attr_mask;      // +6
   uint8_t           attr;           // +7
   void             *ss_draw;        // +8
   uint8_t           res0;           // +10
   uint8_t          *def;            // +11
   uint8_t           res1;           // +13
   uint8_t           res2;           // +14
   uint8_t          *l_def;          // +15
   uint8_t           res3;           // +17
   void             *draw;           // +18
   struct sp1_cs    *next_in_upd;    // +20 BIG ENDIAN
   struct sp1_cs    *prev_in_upd;    // +22 BIG ENDIAN

};

struct sp1_ap {                       // "attribute pairs" - 2 bytes

   uint8_t           attr_mask;      // +0
   uint8_t           attr;           // +1

};

struct sp1_tp {                       // "tile pairs" - 3 bytes

   uint8_t           attr;           // +0
   uint16_t          tile;           // +1

};

struct sp1_pss {                      // "print string struct" - 11 bytes

   struct sp1_Rect  *bounds;         // +0
   uint8_t           flags;          // +2
   uint8_t           x;              // +3
   uint8_t           y;              // +4
   uint8_t           attr_mask;      // +5
   uint8_t           attr;           // +6
   struct sp1_update*pos;            // +7 RESERVED
   void             *visit;          // +9

};

///////////////////////////////////////////////////////////
//                      SPRITES                          //
///////////////////////////////////////////////////////////

// sprite type bits

#define SP1_TYPE_OCCLUDE   0x80
#define SP1_TYPE_BGNDCLR   0x10
#define SP1_TYPE_2BYTE     0x40
#define SP1_TYPE_1BYTE     0x00

// sprite attribute masks

#define SP1_AMASK_TRANS    0xff
#define SP1_AMASK_INK      0xf8
#define SP1_AMASK_PAPER    0xc7
#define SP1_AMASK_NOFLASH  0x7f
#define SP1_AMASK_NOBRIGHT 0xbf
#define SP1_ATTR_TRANS     0x00

// prototype structs

extern struct sp1_cs  sp1_struct_cs_prototype;
extern struct sp1_ss  sp1_struct_ss_prototype;

// draw functions

extern void  SP1_DRAW_MASK2(void);
extern void  SP1_DRAW_MASK2NR(void);
extern void  SP1_DRAW_MASK2LB(void);
extern void  SP1_DRAW_MASK2RB(void);

extern void  SP1_DRAW_LOAD2(void);
extern void  SP1_DRAW_LOAD2NR(void);
extern void  SP1_DRAW_LOAD2LB(void);
extern void  SP1_DRAW_LOAD2RB(void);

extern void  SP1_DRAW_OR2(void);
extern void  SP1_DRAW_OR2NR(void);
extern void  SP1_DRAW_OR2LB(void);
extern void  SP1_DRAW_OR2RB(void);

extern void  SP1_DRAW_XOR2(void);
extern void  SP1_DRAW_XOR2NR(void);
extern void  SP1_DRAW_XOR2LB(void);
extern void  SP1_DRAW_XOR2RB(void);

extern void  SP1_DRAW_LOAD1(void);
extern void  SP1_DRAW_LOAD1NR(void);
extern void  SP1_DRAW_LOAD1LB(void);
extern void  SP1_DRAW_LOAD1RB(void);

extern void  SP1_DRAW_OR1(void);
extern void  SP1_DRAW_OR1NR(void);
extern void  SP1_DRAW_OR1LB(void);
extern void  SP1_DRAW_OR1RB(void);

extern void  SP1_DRAW_XOR1(void);
extern void  SP1_DRAW_XOR1NR(void);
extern void  SP1_DRAW_XOR1LB(void);
extern void  SP1_DRAW_XOR1RB(void);

extern void  SP1_DRAW_ATTR(void);

// sprite functions

extern struct sp1_ss *sp1_CreateSpr(void *drawf, uint8_t type, uint8_t height, uint16_t graphic, uint8_t plane) __sdcccall(1);
extern uint16_t       sp1_AddColSpr(struct sp1_ss *s, void *drawf, uint8_t type, uint16_t graphic, uint8_t plane) __sdcccall(1);
extern void           sp1_ChangeSprType(struct sp1_cs *c, void *drawf) __sdcccall(1);
extern void           sp1_DeleteSpr(struct sp1_ss *s) __sdcccall(1);

extern void           sp1_MoveSprAbs(struct sp1_ss *s, struct sp1_Rect *clip, void *frame, uint8_t row, uint8_t col, uint8_t vrot, uint8_t hrot) __sdcccall(1);
extern void           sp1_MoveSprRel(struct sp1_ss *s, struct sp1_Rect *clip, void *frame, int8_t rel_row, int8_t rel_col, int8_t rel_vrot, int8_t rel_hrot) __sdcccall(1);
extern void           sp1_MoveSprPix(struct sp1_ss *s, struct sp1_Rect *clip, void *frame, uint16_t x, uint16_t y) __sdcccall(1);

extern void           sp1_IterateSprChar(struct sp1_ss *s, void *hook1) __sdcccall(1);
extern void           sp1_IterateUpdateSpr(struct sp1_ss *s, void *hook2) __sdcccall(1);

extern void           sp1_GetSprClrAddr(struct sp1_ss *s, uint8_t **sprdest) __sdcccall(1);
extern void           sp1_PutSprClr(uint8_t **sprdest, struct sp1_ap *src, uint8_t n) __sdcccall(1);
extern void           sp1_GetSprClr(uint8_t **sprsrc, struct sp1_ap *dest, uint8_t n) __sdcccall(1);

extern void          *sp1_PreShiftSpr(uint8_t flag, uint8_t height, uint8_t width, void *srcframe, void *destframe, uint8_t rshift) __sdcccall(1);

extern void           sp1_CreateCharStruct(struct sp1_cs *pTChar, uint8_t *addr, uint8_t x, uint8_t y, uint8_t plane) __sdcccall(1);
extern void           sp1_RemoveCharStruct(struct sp1_cs *cs) __sdcccall(1);

///////////////////////////////////////////////////////////
//                       TILES                           //
///////////////////////////////////////////////////////////

#define SP1_RFLAG_TILE          0x01
#define SP1_RFLAG_COLOUR        0x02
#define SP1_RFLAG_SPRITE        0x04

#define SP1_PSSFLAG_INVALIDATE  0x01
#define SP1_PSSFLAG_XWRAP       0x02
#define SP1_PSSFLAG_YINC        0x04
#define SP1_PSSFLAG_YWRAP       0x08

extern void          *sp1_TileEntry(uint8_t c, void *def) __sdcccall(1);

extern void           sp1_PrintAt(uint8_t row, uint8_t col, uint8_t colour, uint16_t tile) __sdcccall(1);
extern void           sp1_PrintAtInv(uint8_t row, uint8_t col, uint8_t colour, uint16_t tile) __sdcccall(1);
extern uint16_t       sp1_ScreenStr(uint8_t row, uint8_t col) __sdcccall(1);
extern uint8_t        sp1_ScreenAttr(uint8_t row, uint8_t col) __sdcccall(1);

extern void           sp1_PrintString(struct sp1_pss *ps, uint8_t *s) __sdcccall(1);
extern void           sp1_SetPrintPos(struct sp1_pss *ps, uint8_t row, uint8_t col) __sdcccall(1);

extern void           sp1_GetTiles(struct sp1_Rect *r, struct sp1_tp *dest) __sdcccall(1);
extern void           sp1_PutTiles(struct sp1_Rect *r, struct sp1_tp *src) __sdcccall(1);
extern void           sp1_PutTilesInv(struct sp1_Rect *r, struct sp1_tp *src) __sdcccall(1);

extern void           sp1_ClearRect(struct sp1_Rect *r, uint8_t colour, uint8_t tile, uint8_t rflag) __sdcccall(1);
extern void           sp1_ClearRectInv(struct sp1_Rect *r, uint8_t colour, uint8_t tile, uint8_t rflag) __sdcccall(1);

///////////////////////////////////////////////////////////
//                      UPDATER                          //
///////////////////////////////////////////////////////////

#define SP1_IFLAG_MAKE_ROTTBL      0x01
#define SP1_IFLAG_OVERWRITE_TILES  0x02
#define SP1_IFLAG_OVERWRITE_DFILE  0x04

extern void            sp1_Initialize(uint8_t iflag, uint8_t colour, uint8_t tile) __sdcccall(1);
extern void            sp1_UpdateNow(void);

extern void            sp1_IterateUpdateArr(struct sp1_update **ua, void *hook) __sdcccall(1);
extern void            sp1_IterateUpdateRect(struct sp1_Rect *r, void *hook) __sdcccall(1);

extern void            sp1_InvUpdateStruct(struct sp1_update *u) __sdcccall(1);
extern void            sp1_ValUpdateStruct(struct sp1_update *u) __sdcccall(1);

extern void            sp1_DrawUpdateStructIfInv(struct sp1_update *u) __sdcccall(1);
extern void            sp1_DrawUpdateStructIfVal(struct sp1_update *u) __sdcccall(1);
extern void            sp1_DrawUpdateStructIfNotRem(struct sp1_update *u) __sdcccall(1);
extern void            sp1_DrawUpdateStructAlways(struct sp1_update *u) __sdcccall(1);

extern void            sp1_RemoveUpdateStruct(struct sp1_update *u) __sdcccall(1);
extern void            sp1_RestoreUpdateStruct(struct sp1_update *u) __sdcccall(1);

extern void            sp1_Invalidate(struct sp1_Rect *r) __sdcccall(1);
extern void            sp1_Validate(struct sp1_Rect *r) __sdcccall(1);

#endif
