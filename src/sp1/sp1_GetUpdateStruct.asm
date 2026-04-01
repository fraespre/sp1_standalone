INCLUDE "sp1___config.asm"

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
