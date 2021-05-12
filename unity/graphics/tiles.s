;
; Copyright (c) 2020 Anthony Beaucamp.
;
; This software is provided 'as-is', without any express or implied warranty.
; In no event will the authors be held liable for any damages arising from
; the use of this software.
;
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
;
;   1. The origin of this software must not be misrepresented; you must not
;   claim that you wrote the original software. If you use this software in a
;   product, an acknowledgment in the product documentation would be
;   appreciated but is not required.
;
;   2. Altered source versions must be plainly marked as such, and must not
;   be misrepresented as being the original software.
;
;   3. This notice may not be removed or altered from any distribution.
;
;   4. The names of this software and/or it's copyright holders may not be
;   used to endorse or promote products derived from this software without
;   specific prior written permission.
;

	.export _DecodeTiles2x2
	
	.import _tilesetData
	.import _tileCols, _tileRows
	.import _decodeWidth, _decodeHeight
	.import _blockWidth, _charmapWidth

	.segment	"BSS"

_curCol: .res 1

	; Zeropage addresses

.ifdef __APPLE2__
	charPtrZP = $ce
	decPtr1ZP = $fb
	decPtr2ZP = $fd
	tilePtrZP = $ef
	BUFFER:  .res 4
.endif

.ifdef __ATARI__
	charPtrZP = $e2
	decPtr1ZP = $e4
	decPtr2ZP = $e6
	tilePtrZP = $e0
	BUFFER:  .res 4
.endif	
	
.ifdef __CBM__
	charPtrZP = $63
	decPtr1ZP = $fb
	decPtr2ZP = $fd
	tilePtrZP = $61
	BUFFER:  .res 4
.endif	

.ifdef __LYNX__
	charPtrZP = $b5
	decPtr1ZP = $b7
	decPtr2ZP = $b9
	tilePtrZP = $b3
	BUFFER:  .res 4
.endif	

.ifdef __NES__
	charPtrZP = $f0
	decPtr1ZP = $f2
	decPtr2ZP = $f4
	tilePtrZP = $f6
	BUFFER    = $f8
.endif

.ifdef __ATMOS__
	charPtrZP = $b2
	decPtr1ZP = $b4
	decPtr2ZP = $b6
	tilePtrZP = $b0
	BUFFER:  .res 4
.endif

	.segment	"CODE"	

; ---------------------------------------------------------------
; void __near__ _DecodeTiles2x2 (void)
;	Convert Tilemap to Charmap
;	Zero Page Data:
;		tilePtrZP: 16 bit address of tileset data
;		charPtrZP: 16 bit address of location in charmap (auto-updated)
;		decPtr1ZP: 16 bit address of first line (auto-updated)
;		decPtr2ZP: 16 bit address of second line (auto-updated)
; ---------------------------------------------------------------	

_DecodeTiles2x2:

	ldx #0
loopRows: 
	cpx _tileRows
	bpl doneRows
	inx	

		ldy #0
	loopCols:
		cpy _tileCols
		bpl doneCols
		sty _curCol
			
		; Copy 2x2 tile
		jsr processTile
		
		; Move to next col
		ldy _curCol
		iny		
		jmp loopCols	

	doneCols:		
		
		; Update location in charmap
		lda charPtrZP			
		clc	
		adc _charmapWidth
		sta charPtrZP	
		bcc nocarryChrPtr	; Check if carry to high byte
		inc charPtrZP+1
		clc	
	nocarryChrPtr:

		; Update location in decode buffer (line 1)
		lda decPtr1ZP			
		clc	
		adc _blockWidth
		sta decPtr1ZP	
		bcc nocarryScrPtr1	; Check if carry to high byte
		inc decPtr1ZP+1
		clc	
	nocarryScrPtr1:

		; Update location in decode buffer (line 2)
		lda decPtr2ZP		
		clc	
		adc _blockWidth
		sta decPtr2ZP
		bcc nocarryScrPtr2	; Check if carry to high byte
		inc decPtr2ZP+1
	nocarryScrPtr2:
	
	; Move to next row
	jmp loopRows

doneRows:
	rts
	
; ---------------------------------------------------------------	

processTile:
	; Get Tile Value and multiply it by 2 (TileX2)
	lda (charPtrZP),y
	asl A
	sta BUFFER+0

	; Set base address of Tileset
	lda _tilesetData+1
	sta  tilePtrZP+1
	lda _tilesetData
	sta  tilePtrZP

	; Add TileX2 index (2 times)
	clc	
	adc BUFFER+0
	sta tilePtrZP	
	bcc nocarry1
	inc tilePtrZP+1
	clc	
nocarry1:
	clc
	adc BUFFER+0
	sta tilePtrZP	
	bcc nocarry2
	inc tilePtrZP+1
nocarry2:
	
	; Get Tile: 2x2 chars
	ldy #0
	lda (tilePtrZP),y
	sta BUFFER+0
	iny
	lda (tilePtrZP),y
	sta BUFFER+1
	iny
	lda (tilePtrZP),y
	sta BUFFER+2
	iny
	lda (tilePtrZP),y
	sta BUFFER+3
	
	; Assign chars to Decode buffer
	lda _curCol
	asl A
	tay
	lda BUFFER+0
	sta (decPtr1ZP),y
	lda BUFFER+2
	sta (decPtr2ZP),y
	iny
	lda BUFFER+1
	sta (decPtr1ZP),y
	lda BUFFER+3
	sta (decPtr2ZP),y
	
	rts
	