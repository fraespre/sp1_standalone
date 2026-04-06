; ---------------------------------------------------------------------------
; _HEAP : heap pointer variable + pool, lives at 0x5B00 in low RAM
; ---------------------------------------------------------------------------
SECTION _HEAP
	ORG     0x5B00                  ; absolute: HEAP-start 0x5B00, ends 0x66B9; _CRT at 0x66BA

	defc    _heap_data = 0x5B02     ; heap allocation base: past the 2-byte __heap_ptr variable

	PUBLIC  __heap_ptr
	__heap_ptr:
		DEFW    _heap_data          ; initial heap pointer = 0x5B02 (skip the ptr variable itself)
		DEFS    3000                ; heap size 3000b (total 3002b); ends 0x66B9; _CRT at 0x66BA

; ---------------------------------------------------------------------------
; _CRT : entry point + SP init + runtime init 
; ---------------------------------------------------------------------------
SECTION _CRT
	ORG     0x66BA                  ; absolute: entry point

		;ld      sp, 0xD1ED          ; REGISTER_SP = 53741 (behind SP1 struts UPDATELISTH= 0xd1ed)
		ld      sp, 0xD9ED          ; REGISTER_SP = 55798 (size improvement using sp1 Even-Rotation

		ld      hl, 0x5B02          ; init heap pointer: skip the 2-byte __heap_ptr variable itself
		ld      (__heap_ptr), hl

		EXTERN  _main
		call    _main
		; _main never returns

SECTION _SUPPORT
; ---------------------------------------------------------------------------
; Bump allocator
; HL = requested size -> HL = allocated block ptr, CF = 0 on success
; ---------------------------------------------------------------------------
	PUBLIC  __malloc
	__malloc:
		push    de
		ld      d, h                ; DE = size
		ld      e, l
		push    ix
		ld      ix, __heap_ptr       ; IX = address of __heap_ptr variable
		ld      l, (ix+0)           ; HL = current heap ptr
		ld      h, (ix+1)
		push    hl                  ; save old heap ptr for return
		add     hl, de              ; HL = new heap ptr
		ld      (ix+0), l           ; store advanced heap ptr
		ld      (ix+1), h
		pop     hl                  ; HL = old heap ptr = allocated block
		pop     ix
		pop     de
		or      a                   ; clear CF = success
		ret
	
	PUBLIC  __free
	__free:
		ret                         ; no-op in bump allocator

; ---------------------------------------------------------------------------
; SDCC Z80 runtime stub: __moduchar
; --sdcccall 1: A = dividend (1st u8 arg), L = divisor (2nd u8 arg) -> A = remainder
; ---------------------------------------------------------------------------
	PUBLIC  __moduchar
	__moduchar:
		ld      d, l                ; D = divisor (from L, 2nd u8 arg)
	moduchar_loop:
		ld      e, a                ; E = remainder candidate
		sub     d                   ; A = A - divisor
		jr      c, moduchar_done    ; borrow: E holds the remainder
		jr      moduchar_loop
	moduchar_done:
		ld      a, e                ; A = remainder (return value for sdcccall(1))
		ret

; ---------------------------------------------------------------------------
;   _HEAP     0x5B00  heap ptr + pool   [bin 1: sp1__HEAP.bin]
;   _CRT      0x66BA  entry + runtime   [bin 2: sp1__CRT.bin — rest of binary]
;   _RODATA   ~       graphics, tiles    chains after _CRT code
;   _CODE     ~       game logic         chains
;   _DATA     ~       uninit globals     chains
;   _BSS      ~       zero-init          chains
;   _CODE_SP1 ~       SP1 engine asm     chains
;
;   sp1.bin = copy /b sp1__HEAP.bin + sp1__CRT.bin  (no padding needed:
;   HEAP ends at 0x66B9, CRT starts at 0x66BA, exactly adjacent).
; ---------------------------------------------------------------------------
SECTION _RODATA
SECTION _CODE
SECTION _DATA
SECTION _BSS
SECTION _CODE_SP1
