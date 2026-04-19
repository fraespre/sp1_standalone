
/////////////////////////////////////////////////////////////
// EXAMPLE PROGRAM #1
// Ten software-rotated masked sprites move in straight lines
// at various speeds, bouncing off screen edges.  All ten
// share the same sprite graphic which is not animated in
// this first test program.
/////////////////////////////////////////////////////////////

//#include "./sp1_embedded/sp1lib.h"
#include "./sp1_evenrot/sp1lib.h"


static void _asmcode(void) {
  __asm

   defb @11111111, @00000000
   defb @11111111, @00000000
   defb @11111111, @00000000
   defb @11111111, @00000000
   defb @11111111, @00000000
   defb @11111111, @00000000
   defb @11111111, @00000000

   ;GRAPHIC DATA:
   ;Pixel Size:      ( 16,  24)
   ;Char Size:       (  2,   3)
   ;Sort Priorities: Mask, Char line, Y char, X char
   ;Data Outputted:  Gfx
   ;Interleave:      Sprite
   ;Mask:            Yes, before graphic

   _gr_window:
	   DEFB	128,127,  0,192,  0,191, 30,161
	   DEFB	 30,161, 30,161, 30,161,  0,191
	   DEFB	  0,191, 30,161, 30,161, 30,161
	   DEFB	 30,161,  0,191,  0,192,128,127
	   DEFB	255,  0,255,  0,255,  0,255,  0
	   DEFB	255,  0,255,  0,255,  0,255,  0
	
	   DEFB	  1,254,  0,  3,  0,253,120,133
	   DEFB	120,133,120,133,120,133,  0,253
	   DEFB	  0,253,120,133,120,133,120,133
	   DEFB	120,133,  0,253,  0,  3,  1,254
	   DEFB	255,  0,255,  0,255,  0,255,  0
	   DEFB	255,  0,255,  0,255,  0,255,  0
	
  __endasm;
}


// Clipping Rectangle for Sprites
static struct sp1_Rect cr = {0, 0, 32, 24};             // rectangle covering the full screen

// Table Holding Movement Data for Each Sprite
static struct sprentry {
   struct sp1_ss  *s;                            // sprite handle returned by sp1_CreateSpr()
   uint8_t        dx;                            // signed horizontal speed in pixels
   uint8_t        dy;                            // signed vertical speed in pixels
};
static struct sprentry sprtbl[] = {
   {0,1,0}, {0,0,1}, {0,1,2}, {0,2,1}, {0,1,3},
   {0,3,1}, {0,2,3}, {0,3,2}, {0,1,1}, {0,2,2}
};

// A Hashed UDG for Background
static uint8_t hash[] = {0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa};

// Attach C Variable to Sprite Graphics Declared in ASM at End of File
extern uint8_t gr_window[];      // gr_window will hold the address of the asm label _gr_window


void main(void) {
   uint8_t i;
   struct sp1_ss *s;
   struct sprentry *se;

   // disable interrupts
    __asm__("di");

   // Initialize SP1.LIB
   sp1_Initialize(SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE, INK_BLACK | PAPER_WHITE, ' ');
   sp1_TileEntry(' ', hash);   // redefine graphic associated with space character
   sp1_Invalidate(&cr);        // invalidate entire screen so that it is all initially drawn
   sp1_UpdateNow();            // draw screen area managed by sp1 now
   
   // Create Ten Masked Software-Rotated Sprites
   for (i=0; i!=10; i++) {
      s = sprtbl[i].s = sp1_CreateSpr(SP1_DRAW_MASK2LB, SP1_TYPE_2BYTE, 3, 0, i);
      sp1_AddColSpr(s, SP1_DRAW_MASK2, 0, 48, i);
      sp1_AddColSpr(s, SP1_DRAW_MASK2RB, 0, 0, i);
      sp1_MoveSprAbs(s, &cr, gr_window, 10, 14, 0, 4);
   };

   // main loop
   while(1) {
      sp1_UpdateNow();                             // draw screen now
      for (i=0; i!=10; i++) {                       // move all sprites
         se = &sprtbl[i];
         sp1_MoveSprRel(se->s, &cr, 0, 0, 0, se->dy, se->dx);
         if (se->s->row > 21) se->dy = - se->dy;   // if sprite went off screen, reverse direction
         if (se->s->col > 29) se->dx = - se->dx;   // if coord moves less than 0, it becomes 255
      }
   }  

}

