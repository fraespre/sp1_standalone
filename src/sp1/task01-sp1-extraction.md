# Task 01: SP1 Extraction

## Files to Create
- `project/src/cmp/TSP1.h`   - SP1 struct definitions + constants
- `project/src/util/SP1.h`   - extern declarations for asm_sp1_* labels
- `project/src/util/SP1.c`   - all SP1 ASM routines embedded via __asm/__endasm

## Files to Modify
- `project/src/cmp/TRender.h`   - replace `#include <arch/zx/sp1.h>` with `#include "TSP1.h"`
- `project/src/util/RenderUtil.h` - update includes + macros
- `project/src/util/RenderUtil.c` - convert buildSpr/buildChar to __naked ASM; add SP1_init/clearRect/invalidate helpers
- `project/src/util/util.lst` - add SP1.c

## SP1 Routines Used by Game (EXTERN'd in RenderUtil.c)
- asm_sp1_MoveSprPix  → sprites/asm_sp1_MoveSprPix.asm → asm_sp1_MoveSprAbs
- asm_sp1_PrintAt     → tiles/asm_sp1_PrintAt.asm → asm_sp1_GetUpdateStruct
- asm_sp1_TileEntry   → tiles/asm_sp1_TileEntry.asm

## SP1 Routines Called via C in RenderUtil.c (need ASM conversion)
- sp1_CreateSpr    → asm_sp1_CreateSpr   (uses __malloc/__free from heap.asm)
- sp1_AddColSpr    → asm_sp1_AddColSpr
- sp1_InitCharStruct   → asm_sp1_InitCharStruct  (sprites/ not tiles/)
- sp1_GetUpdateStruct  → asm_sp1_GetUpdateStruct
- sp1_InsertCharStruct → asm_sp1_InsertCharStruct (sprites/ not tiles/)
- sp1_InvUpdateStruct  → asm_sp1_InvUpdateStruct

## Macros in RenderUtil.h (need updating)
- RenderUtil_init()        → sp1_Initialize (complex: a=0x05, hl=0x0001)
- RenderUtil_clearRect()   → sp1_ClearRectInv (complex register setup)
- RenderUtil_clearColor()  → sp1_ClearRectInv
- RenderUtil_invalidate()  → sp1_Invalidate (d,e,b,c from SPRITE_AREA)
- RenderUtil_update()      → asm_sp1_UpdateNow() - direct C call (0 params)
- RenderUtil_deleteSpr(s)  → asm_sp1_DeleteSpr(s) __z88dk_fastcall
- RenderUtil_removeChar(c) → asm_sp1_RemoveCharStruct(c) __z88dk_fastcall

## SP1 Draw Functions Needed (sprites/draw/ directory)
Only 4 used: SP1_DRAW_MASK2LB, SP1_DRAW_MASK2, SP1_DRAW_MASK2RB, SP1_DRAW_MASK2NR
Each is a 10-byte data blob that gets copied into sprite structs.

## Key Dependencies
- asm_sp1_ClearRect + ClearRectInv need l_jpix (likely in SP1DrawUpdateStruct.asm)
- asm_sp1_CreateSpr/AddColSpr need __malloc, __free (z88dk heap - kept as-is)
- SP1CRSELECT is PUBLIC in asm_sp1_ClearRect.asm
- _sp1_struct_ss_prototype, _sp1_struct_cs_prototype needed by CreateSpr/AddColSpr

## Calling Convention Pattern in SP1.c
```c
void SP1_routinename_impl(void) __naked {
    __asm
        PUBLIC asm_sp1_RoutineName
        asm_sp1_RoutineName:
        ; Z80 code with:
        ; - config values substituted as literals (e.g. 0xd200 for SP1V_UPDATEARRAY)
        ; - local labels: .RoutineName_labelname
        ; - no SECTION/IF __CLASSIC/INCLUDE directives
        ret
    __endasm;
}
```
