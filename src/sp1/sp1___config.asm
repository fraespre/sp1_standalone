SECTION _CODE_SP1

	; display characteristics
    defc SP1V_DISPORIGX   = 0		; x coordinate of top left corner of area managed by sp1 in characters
    defc SP1V_DISPORIGY   = 0		; y coordinate of top left corner of area managed by sp1 in characters
    defc SP1V_DISPWIDTH   = 32		; width of area managed by sp1 in characters (16, 24, 32 ok as of now)
    defc SP1V_DISPHEIGHT  = 24		; height of area managed by sp1 in characters
	
	; sp1 variables
	defc SP1V_UPDATELISTH = 0xD1ED	; address of 10-byte area holding a dummy struct_sp1_update that is always the "first" in list of screen tiles to be drawn
    defc SP1V_UPDATELISTT = 0xD1EF	; address of 2-byte variable holding the address of the last struct_sp1_update in list of screen tiles to be drawn
									; note: SP1V_UPDATELISTT is located inside the dummy struct_sp1_update pointed at by SP1V_UPDATELISTH
	; buffers
	defc SP1V_PIXELBUFFER = 0xD1F7	; address of an 8-byte buffer to hold intermediate pixel-draw results
    defc SP1V_ATTRBUFFER  = 0xD1FF	; address of a single byte buffer to hold intermediate colour-draw results
	
	; data structure locations
    defc SP1V_UPDATEARRAY = 0xD200	; address of the 10*SP1V_DISPWIDTH*SP1V_DISPHEIGHT byte update array
    defc SP1V_TILEARRAY   = 0xF000	; address of the 512-byte tile array associating character codes with tile graphics, must lie on 256-byte boundary (LSB=0)
    defc SP1V_ROTTBL      = 0xF000	; location of the 3584-byte rotation table.  Must lie on 256-byte boundary (LSB=0).  Table begins $0200 bytes ahead of this
                                    ;  pointer ($f200-$ffff in this default case). Set to $0000 if the table is not needed (if, for example, all sprites are
                                    ;  drawn at exact horizontal character coordinates or you use pre-shifted sprites only)
									