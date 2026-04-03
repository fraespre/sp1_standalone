# Plan: SP1 Rotation Table Compaction (even-pixel movement)

## Context

SP1 normally builds 7 rotation tables (hrot=1..7), each 512 bytes, at addresses
`((hrot<<1) | (SP1V_ROTTBL/256)) << 8`. With 2-pixel movement sprites never reach
hrot=1,3,5,7 — only hrot=0,2,4,6 are used (hrot=0 takes the NR path, no table needed).

The goal is to:
1. Emit only the 3 needed tables (hrot=2,4,6), making them contiguous
2. Move all SP1 data structures up by 2048 bytes, freeing that range for game use

> **Note:** The documentation cited says 1536 bytes (3 tables), but the actual
> saving is **2048 bytes** (4 odd tables × 512). By changing the addressing formula
> (shift replaced by OR), the tables land at 0xFA00–0xFFFF and TILEARRAY can share
> the ROTTBL base at 0xF800, eliminating a 512-byte gap. Net freed = 2048 bytes.

---

## New Memory Layout

| Address | Size | Content |
|---------|------|---------|
| 0xD1ED–0xD9EC | **2048** | **FREE** (was SP1 data, now game RAM/stack) |
| 0xD9ED | 2 | SP1V_UPDATELISTH |
| 0xD9EF | 2 | SP1V_UPDATELISTT |
| 0xD9F7 | 8 | SP1V_PIXELBUFFER |
| 0xD9FF | 1 | SP1V_ATTRBUFFER |
| 0xDA00–0xF7FF | 7680 | SP1V_UPDATEARRAY (ends exactly at 0xF800) |
| 0xF800–0xF9FF | 512 | SP1V_TILEARRAY (= SP1V_ROTTBL base) |
| 0xFA00–0xFBFF | 512 | hrot=2 table (lo:0xFA, hi:0xFB) |
| 0xFC00–0xFDFF | 512 | hrot=4 table (lo:0xFC, hi:0xFD) |
| 0xFE00–0xFFFF | 512 | hrot=6 table (lo:0xFE, hi:0xFF) |

SP1V_TILEARRAY and SP1V_ROTTBL unify at 0xF800. Safe: DrawUpdateStruct reads tile data
from 0xF8xx (H=0xF8); draw functions detect NR by `cp 0xF8` — separate code paths.

---

## Files Changed (5 total)

### 1. `src/sp1/sp1___config.asm`

New constants (everything +0x800):

```asm
defc SP1V_UPDATELISTH = 0xd9ed   ; was 0xd1ed
defc SP1V_UPDATELISTT = 0xd9ef   ; was 0xd1ef
defc SP1V_PIXELBUFFER = 0xd9f7   ; was 0xd1f7
defc SP1V_ATTRBUFFER  = 0xd9ff   ; was 0xd1ff
defc SP1V_UPDATEARRAY = 0xda00   ; was 0xd200
defc SP1V_TILEARRAY   = 0xf800   ; was 0xf000
defc SP1V_ROTTBL      = 0xf800   ; was 0xf000
```

`SP1V_DISPORIGX/Y/WIDTH/HEIGHT` unchanged.

All 30 SP1 .asm files include `sp1___config.asm`, so `SP1V_ROTTBL/256` (NR detection
in DRAW_MASK2*.asm), `SP1V_PIXELBUFFER+N` (DRAW_MASK2*.asm), `SP1V_TILEARRAY/256`
(DrawUpdateStruct.asm), etc. all update automatically — **no changes to those files**.

---

### 2. `src/sp1/sp1_Initialize.asm` (rotation table loop, lines 12–43)

Old loop (c=7 down to 1, formula `(c<<1)|0xF0`):
```asm
   ld c,7
   push af
.rottbllp
   ld a,c
   add a,a           ; a = c*2  ← REMOVE THIS LINE
   or SP1V_ROTTBL/256
   ld h,a
   ld l,0
.entrylp
   ld b,c
   ...
   dec c
   jp nz, rottbllp   ← only one dec c
   pop af
```

New loop (c=6,4,2, formula `c|0xF8`):
```asm
   ld c,6            ; was 7: start with hrot=6
   push af
.rottbllp
   ld a,c
   or SP1V_ROTTBL/256 ; 6|0xF8=0xFE, 4|0xF8=0xFC, 2|0xF8=0xFA
   ld h,a
   ld l,0
.entrylp
   ld b,c            ; shift count = actual hrot (6, 4, 2)
   ...
   dec c
   dec c             ; ADD: step by -2 (skip odd hrot)
   jp nz, rottbllp
   pop af
```

Three edits: `ld c,7` → `ld c,6`; remove `add a,a`; add `dec c` before `jp nz, rottbllp`.

The inner loop uses `b=c` as shift count. With c=6,4,2 this is still the correct
rotation amount (the old loop also used raw c as shift count). ✓

Loop exit: c=2 → dec→1 → dec→0 → jp nz not taken → exits ✓

---

### 3. `src/sp1/sp1_MoveSprAbs.asm` (e_hrot encoding, lines 15–16)

Old (2 instructions):
```asm
   add a,a                  ; a = hrot*2
   add a,SP1V_ROTTBL/256    ; a = (hrot<<1)|0xF0
   ld (ix+9),a
```

New (1 instruction):
```asm
   or  SP1V_ROTTBL/256      ; a = hrot|0xF8 (0xF8, 0xFA, 0xFC, 0xFE)
   ld (ix+9),a
```

NR detection still works: hrot=0 → 0|0xF8=0xF8=SP1V_ROTTBL/256 → `cp` triggers NR ✓

---

### 4. `src/sp1/sp1_InitCharStruct.asm` (line 61)

The embedded sprite stub hardcodes e_hrot = `SP1V_ROTTBL/256 + 8` (= hrot=4 in
old formula). With new formula hrot=4 → 4|0xF8=0xFC=SP1V_ROTTBL/256+4.

Old:
```asm
   ld a,SP1V_ROTTBL/256 + 8    ; was 0xF0+8=0xF8 = old hrot=4 H
```

New:
```asm
   ld a,SP1V_ROTTBL/256 + 4    ; new 0xF8+4=0xFC = new hrot=4 H
```

Without this fix: `0xF8+8=0x100` → loads A=0x00 → e_hrot=0 → sprites always NR-drawn.

---

### 5. `src/crt0.asm` (line 20)

Stack pointer must be initialised to the bottom of SP1 data (stack grows downward).

Old:
```asm
   ld sp, 0xD1ED     ; was bottom of SP1
```

New:
```asm
   ld sp, 0xD9ED     ; new bottom of SP1 (= new SP1V_UPDATELISTH)
```

---

## Files NOT Changed

- `sp1_DRAW_MASK2.asm`, `sp1_DRAW_MASK2LB.asm`, `sp1_DRAW_MASK2RB.asm`,
  `sp1_DRAW_MASK2NR.asm` — all use symbolic `SP1V_ROTTBL/256` and `SP1V_PIXELBUFFER+N`
  → auto-updated via config include
- `sp1_MoveSprPix.asm`, `sp1_MoveSprRel.asm` — both tail-call `asm_sp1_MoveSprAbs`
- All other 25 SP1 .asm files — reference config constants only

---

## Verification

1. `ninja` → clean build (no errors expected)
2. Run `sp1.tap` in emulator → sprites move correctly in 2-pixel steps
3. Check map file: SP1V_UPDATELISTH at 0xD9ED, TILEARRAY/ROTTBL at 0xF800
4. Confirm sprites bounce correctly (all hrot=0,2,4,6 paths exercised)
5. Confirm no corruption: hrot=0 → NR path (no table read), hrot=2,4,6 → table lookup
