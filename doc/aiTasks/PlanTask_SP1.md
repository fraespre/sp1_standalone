# Plan: Task01 — Extract SP1 from z88dk into project source tree

## Context
The game uses z88dk's bundled SP1 sprite library. To break the z88dk dependency, SP1 must be
extracted into the project's own `src/sp1/` directory. The extracted SP1 will consist of:
- 26 Z80 ASM files copied (and adapted) from z88dk
- A config/constants file (`sp1_customize.asm`)
- A single header (`src/sp1/SP1.h`) with all structs, flags, and function declarations
- A C wrapper layer (`src/sp1/SP1.c`)
- A `build.ninja` update for the new compile target
- Minor updates to `src/cmp/TRender.h` and `src/util/RenderUtil.h` to reference the new header

**Note on location discrepancy:** `ReqPlanTask01.txt` says SP1.h/SP1.c go in `src/util/`, but
existing code (`RenderUtil.h` line 7: `#include "../sp1/SP1.h"`) and `build.ninja` (`src/sp1/SP1.c`)
already reference `src/sp1/`. The plan follows the existing code — SP1.h and SP1.c live in `src/sp1/`.

---

## Key Paths
- z88dk SP1 ASM source: `toolStack/z88dk-20250129/libsrc/_DEVELOPMENT/temp/sp1/zx/`
- z88dk config template: `toolStack/z88dk-20250129/libsrc/sprites/software/sp1/zx/config_private.inc`
- z88dk reference header: `toolStack/z88dk-20250129/libsrc/sprites/software/sp1/deprecated/spectrum/spectrum-sp1.h`
- Project SP1 folder: `project/src/sp1/` (currently empty)
- `project/build.ninja` (already has 26 ASM targets + SP1.o, needs sp1_customize.o added)
- `project/src/cmp/TRender.h` (currently `#include "TSP1.h"` → change to `#include "../sp1/SP1.h"`)
- `project/src/util/RenderUtil.h` (currently includes both `TSP1.h` and `SP1.h` → drop the TSP1.h include, SP1.h covers it)

---

## Step 1 — Create `src/sp1/sp1_customize.asm`

Replaces z88dk's `config_private.inc`. Contains `defc` constants + `SECTION code_temp_sp1`.
Each extracted SP1 ASM file will `INCLUDE "sp1_customize.asm"` in place of `INCLUDE "config_private.inc"`.

```asm
SECTION code_temp_sp1

defc SP1V_DISPORIGX   = 0
defc SP1V_DISPORIGY   = 0
defc SP1V_DISPWIDTH   = 32
defc SP1V_DISPHEIGHT  = 24
defc SP1V_PIXELBUFFER = 0xd1f7
defc SP1V_ATTRBUFFER  = 0xd1ff
defc SP1V_TILEARRAY   = 0xf000
defc SP1V_UPDATEARRAY = 0xd200
defc SP1V_ROTTBL      = 0xf000
defc SP1V_UPDATELISTH = 0xd1ed
defc SP1V_UPDATELISTT = 0xd1ef
```

---

## Step 2 — Copy and adapt 26 ASM files into `src/sp1/`

### File mapping (z88dk source → project target)

| z88dk source path (relative to `zx/`) | Target in `src/sp1/` |
|---|---|
| `updater/SP1DrawUpdateStruct.asm` | `sp1_DrawUpdateStruct.asm` |
| `updater/asm_sp1_GetUpdateStruct.asm` | `sp1_GetUpdateStruct.asm` |
| `updater/asm_sp1_InvUpdateStruct.asm` | `sp1_InvUpdateStruct.asm` |
| `updater/asm_sp1_UpdateNow.asm` | `sp1_UpdateNow.asm` |
| `updater/asm_sp1_Invalidate.asm` | `sp1_Invalidate.asm` |
| `updater/asm_sp1_Initialize.asm` | `sp1_Initialize.asm` |
| `tiles/asm_sp1_TileEntry.asm` | `sp1_TileEntry.asm` |
| `tiles/asm_sp1_PrintAt.asm` | `sp1_PrintAt.asm` |
| `tiles/asm_sp1_ClearRect.asm` | `sp1_ClearRect.asm` |
| `tiles/asm_sp1_ClearRectInv.asm` | `sp1_ClearRectInv.asm` |
| `sprites/__sp1_add_spr_char.asm` | `sp1__add_spr_char.asm` |
| `sprites/__sp1_remove_spr_char.asm` | `sp1__remove_spr_char.asm` |
| `sprites/_sp1_struct_ss_prototype.asm` | `sp1__struct_ss_prototype.asm` |
| `sprites/_sp1_struct_cs_prototype.asm` | `sp1__struct_cs_prototype.asm` |
| `sprites/draw/SP1_DRAW_MASK2.asm` | `sp1_draw_mask2.asm` |
| `sprites/draw/SP1_DRAW_MASK2NR.asm` | `sp1_draw_mask2nr.asm` |
| `sprites/draw/SP1_DRAW_MASK2LB.asm` | `sp1_draw_mask2lb.asm` |
| `sprites/draw/SP1_DRAW_MASK2RB.asm` | `sp1_draw_mask2rb.asm` |
| `sprites/asm_sp1_CreateSpr.asm` | `sp1_CreateSpr.asm` |
| `sprites/asm_sp1_AddColSpr.asm` | `sp1_AddColSpr.asm` |
| `sprites/asm_sp1_DeleteSpr.asm` | `sp1_DeleteSpr.asm` |
| `sprites/asm_sp1_MoveSprAbs.asm` | `sp1_MoveSprAbs.asm` |
| `sprites/asm_sp1_MoveSprPix.asm` | `sp1_MoveSprPix.asm` |
| `sprites/asm_sp1_InitCharStruct.asm` | `sp1_InitCharStruct.asm` |
| `sprites/asm_sp1_InsertCharStruct.asm` | `sp1_InsertCharStruct.asm` |
| `sprites/asm_sp1_RemoveCharStruct.asm` | `sp1_RemoveCharStruct.asm` |

**Note:** Check if `asm_sp1_MoveSprAbs.asm` / `asm_sp1_MoveSprPix.asm` reference internal helpers
`__sp1_move_c` or `__sp1_move_nc` (from `sprites/__sp1_move_c.asm`, `__sp1_move_nc.asm`).
If so, add those two files + build.ninja entries as well.

### Adaptations to apply to every copied file

1. **Replace include:** `INCLUDE "config_private.inc"` → `INCLUDE "sp1_customize.asm"`
2. **Remove `SECTION code_clib`** line if present (keep only `SECTION code_temp_sp1`)
3. **Apostrophe in alternate-register instructions** — in standalone .asm files the C preprocessor
   does NOT run, so `ex af,af'` and `af'` in comments are fine. However, sdasZ80 may require
   `ex af,af` with no apostrophe in the mnemonic — verify and apply `.db 0x08` if a compile error occurs.
4. **Uppercase label dot-prefix** in instruction operands: sdasZ80 rejects `.LABEL` in operands —
   strip the dot from uppercase labels used as jump targets in operand position.
5. **`ld (hl), label / 256`** pattern: sdasZ80 bakes section-relative byte — replace with
   `ld de, label` / `ld (hl), d` as needed.
6. **SP1CRSELECT table lookup** in CreateSpr (if present): replace byte arithmetic with
   `ld hl, table; add hl, de`.

### Known bugs already identified (from prior work — see MEMORY.md)

- **`sp1_CreateSpr.asm`** struct init section:
  - `ld (iy+4), h` must be `ld (iy+4), c` — plane is in C for CreateSpr
  - Missing `ld (iy+5), a` — type must be stored
  - Draw-code copy must match z88dk source exactly
  - Graphics ptr pop order was wrong — must match z88dk
- **`sp1_AddColSpr.asm`**: plane IS in H — `ld (iy+4), h` is correct (different convention from CreateSpr).

---

## Step 3 — Create `src/sp1/SP1.h`

Based on `spectrum-sp1.h`. Replaces z88dk's SP1 header entirely — contains all struct definitions
and function declarations in one file (no TSP1.h dependency).

Use `uint8_t` / `uint16_t` (from `<stdint.h>`) replacing z88dk's `uchar` / `uint`.
Field layout of every struct must match z88dk exactly (offsets are used in ASM).

### Calling convention rules for SP1.h declarations

Every function declaration must use one of these SDCC conventions (no `__LIB__`, `__CALLEE__`, or
`__FASTCALL__` z88dk macros):

| Case | Convention | When to use |
|---|---|---|
| All functions with arguments | `__sdcccall(1)` | Every function that takes parameters |
| No arguments | (none) | UpdateNow |

The `__sdcccall(1)` convention passes the **first** argument in `HL` (or `HL`+`DE` for wider
types) and remaining arguments on the stack. The underlying `asm_sp1_*` routines already use the
z88dk callee convention, so the C wrappers in SP1.c simply `jp asm_sp1_*` — the ASM routine pops
its own stack frame before returning.

### SP1.h structure

```c
#ifndef _SP1
#define _SP1
#include <stdint.h>

// --- Data structures ---
struct sp1_Rect   { uint8_t row; uint8_t col; uint8_t width; uint8_t height; };
struct sp1_update { ... };   // 10 bytes — full field layout from spectrum-sp1.h
struct sp1_ss     { ... };   // 20 bytes
struct sp1_cs     { ... };   // 24 bytes
struct sp1_ap     { uint8_t attr_mask; uint8_t attr; };
struct sp1_tp     { uint8_t attr; uint16_t tile; };
struct sp1_pss    { ... };   // 11 bytes

// --- Sprite type flags ---
#define SP1_TYPE_OCCLUDE        0x80
#define SP1_TYPE_BGNDCLR        0x10
#define SP1_TYPE_2BYTE          0x40
#define SP1_TYPE_1BYTE          0x00

// --- Attribute masks ---
#define SP1_AMASK_TRANS         0xff
#define SP1_AMASK_INK           0xf8
#define SP1_AMASK_PAPER         0xc7
#define SP1_AMASK_NOFLASH       0x7f
#define SP1_AMASK_NOBRIGHT      0xbf
#define SP1_ATTR_TRANS          0x00

// --- Rect / tile flags ---
#define SP1_RFLAG_TILE          0x01
#define SP1_RFLAG_COLOUR        0x02
#define SP1_RFLAG_SPRITE        0x04

// --- PrintString flags ---
#define SP1_PSSFLAG_INVALIDATE  0x01
#define SP1_PSSFLAG_XWRAP       0x02
#define SP1_PSSFLAG_YINC        0x04
#define SP1_PSSFLAG_YWRAP       0x08

// --- Initialize flags ---
#define SP1_IFLAG_MAKE_ROTTBL      0x01
#define SP1_IFLAG_OVERWRITE_TILES  0x02
#define SP1_IFLAG_OVERWRITE_DFILE  0x04

// --- Prototype structs (defined in ASM files) ---
extern struct sp1_cs sp1_struct_cs_prototype;
extern struct sp1_ss sp1_struct_ss_prototype;

// --- Draw function externs (only MASK2 variants used by game) ---
extern void SP1_DRAW_MASK2(void);
extern void SP1_DRAW_MASK2NR(void);
extern void SP1_DRAW_MASK2LB(void);
extern void SP1_DRAW_MASK2RB(void);

// --- Function declarations ---
struct sp1_ss*     sp1_CreateSpr(void *drawf, uint8_t type, uint8_t height, int16_t graphic, uint8_t plane) __sdcccall(1);
uint16_t           sp1_AddColSpr(struct sp1_ss *s, void *drawf, uint8_t type, int16_t graphic, uint8_t plane) __sdcccall(1);
void               sp1_DeleteSpr(struct sp1_ss *s) __sdcccall(1);
void               sp1_MoveSprAbs(struct sp1_ss *s, struct sp1_Rect *clip, void *frame, uint8_t row, uint8_t col, uint8_t vrot, uint8_t hrot) __sdcccall(1);
void               sp1_MoveSprPix(struct sp1_ss *s, struct sp1_Rect *clip, void *frame, uint16_t x, uint16_t y) __sdcccall(1);
void               sp1_InitCharStruct(struct sp1_cs *cs, void *drawf, uint8_t type, void *graphic, uint8_t plane) __sdcccall(1);
void               sp1_InsertCharStruct(struct sp1_update *u, struct sp1_cs *cs) __sdcccall(1);
void               sp1_RemoveCharStruct(struct sp1_cs *cs) __sdcccall(1);
void*              sp1_TileEntry(uint8_t c, void *def) __sdcccall(1);
void               sp1_PrintAt(uint8_t row, uint8_t col, uint8_t colour, uint16_t tile) __sdcccall(1);
void               sp1_ClearRect(struct sp1_Rect *r, uint8_t colour, uint8_t tile, uint8_t rflag) __sdcccall(1);
void               sp1_ClearRectInv(struct sp1_Rect *r, uint8_t colour, uint8_t tile, uint8_t rflag) __sdcccall(1);
void               sp1_Initialize(uint8_t iflag, uint8_t colour, uint8_t tile) __sdcccall(1);
void               sp1_UpdateNow(void);
struct sp1_update* sp1_GetUpdateStruct(uint8_t row, uint8_t col) __sdcccall(1);
void               sp1_InvUpdateStruct(struct sp1_update *u) __sdcccall(1);
void               sp1_Invalidate(struct sp1_Rect *r) __sdcccall(1);

#endif
```

---

## Step 4 — Create `src/sp1/SP1.c`

C wrappers following the same `__naked __sdcccall(1)` pattern used in `RenderUtil.c`.

### Calling convention rules for SP1.c implementations

- All wrappers use `__naked` — no compiler-generated prologue/epilogue.
- `__naked` must be placed **after** the parameter list, not before the function name:
  - CORRECT: `void sp1_Foo(uint8_t a) __naked __sdcccall(1)`
  - WRONG:   `void __naked sp1_Foo(uint8_t a) __sdcccall(1)`
- Body: dummy parameter references to suppress warnings + `__asm EXTERN ... jp asm_sp1_* __endasm;`
- `EXTERN` must be declared inside the `__asm` block before its first use.
- Apostrophes in `__asm` comments crash the C preprocessor — use `a_alt`, `bc_alt` instead of `af'`, `bc'`.

**Multi-arg wrapper pattern (`__sdcccall(1)`):**
```c
void sp1_Initialize(uint8_t iflag, uint8_t colour, uint8_t tile) __naked __sdcccall(1) {
    iflag; colour; tile;
    __asm
        EXTERN asm_sp1_Initialize
        jp asm_sp1_Initialize
    __endasm;
}
```

**Single-arg wrapper (`__sdcccall(1)`):**
```c
void sp1_Invalidate(struct sp1_Rect *r) __naked __sdcccall(1) {
    r;
    __asm
        EXTERN asm_sp1_Invalidate
        jp asm_sp1_Invalidate
    __endasm;
}
```

The `jp asm_sp1_*` works because the underlying ASM routine uses callee convention — it pops
its own parameters and returns directly to the original caller.

SP1.c must open with:
```c
#include "SP1.h"
```

---

## Step 5 — Update existing headers

### `src/cmp/TRender.h`
Change line 5 from:
```c
#include "TSP1.h"
```
to:
```c
#include "../sp1/SP1.h"
```
Typedefs for `TSprite`, `TUpdate`, `TChar` (lines 9-11) remain unchanged — they reference
`struct sp1_ss`, `struct sp1_update`, `struct sp1_cs` which are now defined in SP1.h.

### `src/util/RenderUtil.h`
Remove line 6:
```c
#include "../cmp/TSP1.h"
```
SP1.h (already on line 7) now provides all struct definitions.

---

## Step 6 — Update `build.ninja`

Add one compile rule and link entry for `sp1_customize.asm`:

```ninja
build $buildir/sp1_customize.o:    compile src/sp1/sp1_customize.asm
build $buildir/sp1_draw_mask2.o:   compile src/sp1/sp1_draw_mask2.asm
build $buildir/sp1_draw_mask2nr.o: compile src/sp1/sp1_draw_mask2nr.asm
build $buildir/sp1_draw_mask2lb.o: compile src/sp1/sp1_draw_mask2lb.asm
build $buildir/sp1_draw_mask2rb.o: compile src/sp1/sp1_draw_mask2rb.asm
```

Replace the existing `SP1_DRAW_MASK2*.o` entries in both the compile section and the link line
with the renamed targets above.

If `sp1_move_c.asm` / `sp1_move_nc.asm` are needed (see Step 2 note), also add:
```ninja
build $buildir/sp1_move_c.o:  compile src/sp1/sp1_move_c.asm
build $buildir/sp1_move_nc.o: compile src/sp1/sp1_move_nc.asm
```
and include those in the link line.

---

## Verification

1. `cd project && ninja` — build must complete with zero errors.
2. Confirm all `asm_sp1_*` symbols in `build/rovin.map` resolve to the `code_temp_sp1` section
   (not to any z88dk library code).
3. Run `build/rovin.tap` in a ZX Spectrum emulator (e.g. Fuse) — sprites, tiles, and screen updates
   must work identically to before the extraction.
4. Verify `code_temp_sp1` section placement in `rovin.map` falls within the expected address range.
