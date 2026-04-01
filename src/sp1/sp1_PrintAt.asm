INCLUDE "sp1___config.asm"

PUBLIC asm_sp1_PrintAt

EXTERN asm_sp1_GetUpdateStruct

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
