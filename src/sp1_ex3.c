/****************************************
 sp1 Sprite Demo 1
 A simple demo program to show how z88dk and sp1 can be used to make a game.
http://www.worldofspectrum.org/forums/discussion/37467/a-z88dk-sp1-demo-with-source
http://www.worldofspectrum.org/forums/discussion/comment/864043/#Comment_864043
****************************************/

#include "./sp1/sp1lib.h"


static void _asmcode(void) {
  __asm

_backwalls:
; bricks
  DEFB 247,  247,  247,    0,  223,  223,  223,    0

; cirkel muur
  DEFB 248,  227,  207,  159,  191,   63,  127,  127 
  DEFB 127,  127,   63,  191,  159,  207,  227,  248 

  DEFB  31,  199,  243,  249,  253,  252,  254,  254 
  DEFB 254,  254,  252,  253,  249,  243,  199,   31 

;; SPRITES are defined here
  DEFB 255,    0,  255,    0,  255,    0,  255,    0
  DEFB 255,    0,  255,    0,  255,    0,  255,    0

_sprite1:
  DEFB 248,    0,  224,    3,  192,   12,  128,   16
  DEFB 128,   32,    0,   32,    0,   64,    0,   64
  DEFB   0,   70,    0,   73,    0,   75,    0,   70
  DEFB   0,    0,    0,  127,    0,    6,  240,    0
  DEFB 255,    0,  255,    0,  255,    0,  255,    0
  DEFB 255,    0,  255,    0,  255,    0,  255,    0

  DEFB  31,    0,   15,  192,    3,   48,    1,    8
  DEFB   1,  196,    0,   36,    0,   18,    0,   18
  DEFB   0,  210,    0,   34,    0,   98,    0,  194
  DEFB   0,    0,    0,  234,    0,  160,   15,    0
  DEFB 255,    0,  255,    0,  255,    0,  255,    0
  DEFB 255,    0,  255,    0,  255,    0,  255,    0
	
  __endasm;
}


// Clipping Rectangle for Sprites
// format is y,x,w,h!
static struct sp1_Rect cliprect    = {2, 2, 28, 20};             // full screen minus border
static struct sp1_Rect levelbrect  = {0, 0, 32, 24};             // full screen


// The maximum number of sprites is 10.
static uint8_t maxsprites = 9;


// Table Holding Movement Data for Each Sprite
static struct sprentry {
   struct sp1_ss  *s;          // sprite handle returned by sp1_CreateSpr()
   uint8_t   x;          // sprite x coordinate
   uint8_t   y;          // sprite x coordinate
   uint8_t  dx;          // signed horizontal speed in pixels
   uint8_t  dy;          // signed vertical speed in pixels
};


// Define Starting Positions of the Sprites
static struct sprentry sprtbl[] = {
   {0,128,88,1,1}, {0,128,120,2,0}, {0,128,56,2,0}, {0,96,88,2,0}, {0,96,120,2,0},
   {0,96,56,1,1}, {0,160,88,2,0}, {0,160,120,2,0}, {0,160,56,2,0}, {0,0,0,2,0}
};


// Attach C Variable to data declared in external asm file "graphics.asm"
extern uint8_t sprite1[];
extern uint8_t backwalls[];


// Memory to define the background.
static uint8_t levelb[32*24*3];


// Refresh the whole screen
static void drawmap(void) {
   sp1_PutTiles(&levelbrect, (struct sp1_tp *)(levelb));
   sp1_Invalidate(&levelbrect);
}

static uint8_t random(void) __naked __sdcccall(1) { //call-convention uses reg.A as return 8bits
  __asm
      ld A, R 
      ret	
  __endasm;
}

// Fill the background with some arbitrary patterns, and draw it.
void loadlevel(void) {
   uint16_t p;
   uint16_t i;

   p = 0;
   i = 0;
   while (i<32*24) {
      levelb[p+1] = 108+(random()&3);
      levelb[p]   = 68+(random()&3);
      if ((i<64) || (i>703) || ((i&31)>29) || ((i&31)<2)) {
         levelb[p+1] =107;
         levelb[p]   =86;
      }
      i++;
      p += 3;
   }
   p=p-15+1;
   
   levelb[p]=84;
   levelb[p+3]=73;
   levelb[p+6]=77;
   levelb[p+9]=77;
   levelb[p+12]=89;
   drawmap();
}



// Main: Program Starts HERE
void main(void) {
   uint8_t i;
   struct sp1_ss *s;

   // disable interrupts
    __asm__("di");

   // The next part is very important. The game will not compile if it is removed.
   // Initialise the SP1 library   
   sp1_Initialize( SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE, INK_WHITE | PAPER_BLACK, ' ');

   // Initialise Background Tiles
   for (i=0; i<5; i++) {
      sp1_TileEntry(107+i, backwalls+i*8);
   }

   // Call SP1 functions to let it know we are using 10 sprites.
   for (i=0; i<maxsprites; i++) {
      s = sprtbl[i].s = sp1_CreateSpr(SP1_DRAW_MASK2LB, SP1_TYPE_2BYTE, 3, 0, 0);
      sp1_AddColSpr(s, SP1_DRAW_MASK2, 0, 48, i);
      sp1_AddColSpr(s, SP1_DRAW_MASK2RB, 0, 0, i);
   };

   // Create some random directions for our sprites.
   for (i=0; i<10; i++) {
      sprtbl[i].dx = (i-5); sprtbl[i].dy = (i-5);
      if (i==5) { sprtbl[i].dx = 1; sprtbl[i].dy = 1; }
   }

   // Draw some random background
   loadlevel();

   // MAIN LOOP
   while(1) {

      // Let SP1 know where the sprites are located.
      for (i=0; i<maxsprites; i++) {
         sp1_MoveSprPix(sprtbl[i].s, &cliprect, sprite1, sprtbl[i].x, sprtbl[i].y);
      }

      // Update Sprites
      sp1_UpdateNow();

      // Calculate next location of sprites.
      // If sprite is outside the screen then change direction.
      for (i=0; i<maxsprites; i++) {
         sprtbl[i].x += sprtbl[i].dx;
         sprtbl[i].y += sprtbl[i].dy;
         
         if (sprtbl[i].x>240) sprtbl[i].dx=-sprtbl[i].dx;
         if (sprtbl[i].y>176) sprtbl[i].dy=-sprtbl[i].dy;
      }
   }

}
