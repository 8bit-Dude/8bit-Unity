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

	.export _DecodeTiles
	
	.import _tileCols, _tileRows
	.import _decodeWidth, _decodeHeight
	.import _blockWidth, _charmapWidth

; Zeropage addresses

.ifdef __APPLE2__
	tilesetDataZP = $ef
	charmapDataZP = $ce
	decodeLine1ZP = $fb
	decodeLine2ZP = $fd
.endif

.ifdef __ATARI__
	tilesetDataZP = $e0
	charmapDataZP = $e2
	decodeLine1ZP = $e4
	decodeLine2ZP = $e6
.endif	
	
.ifdef __CBM__
	tilesetDataZP = $61
	charmapDataZP = $63
	decodeLine1ZP = $fb
	decodeLine2ZP = $fd
.endif	

.ifdef __LYNX__
	tilesetDataZP = $b3
	charmapDataZP = $b5
	decodeLine1ZP = $b7
	decodeLine2ZP = $b9
.endif	

.ifdef __ATMOS__
	tilesetDataZP = $ef
	charmapDataZP = $b5
	decodeLine1ZP = $b7
	decodeLine2ZP = $b9
.endif

	.segment	"BSS"

_tmpChr: .res 4
_curCol: .res 1
_curRow: .res 1

	.segment	"CODE"	

; ---------------------------------------------------------------
; void __near__ _DecodeTiles (void)
;	Convert Tilemap to Charmap
;	Zero Page Data:
;		tilesetDataZP: 16 bit address of tileset data
;		charmapDataZP: 16 bit address of location on charmap (auto-updated)
;		decodeLine1ZP: 16 bit address of first screen line (auto-updated)
;		decodeLine2ZP: 16 bit address of second screen line (auto-updated)
; ---------------------------------------------------------------	

.proc _DecodeTiles: near

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
		
			; Get Tile Value (x4)
			lda (charmapDataZP),y
			asl A
			asl A
			
			; Get All Chars
			tay
			lda (tilesetDataZP),y
			sta _tmpChr+0
			iny
			lda (tilesetDataZP),y
			sta _tmpChr+1
			iny
			lda (tilesetDataZP),y
			sta _tmpChr+2
			iny
			lda (tilesetDataZP),y
			sta _tmpChr+3
			
			; Assign Chars to Buffer (4 bytes per Tile)
			lda _curCol
			asl A
			tay
			lda _tmpChr+0
			sta (decodeLine1ZP),y
			lda _tmpChr+2
			sta (decodeLine2ZP),y
			iny
			lda _tmpChr+1
			sta (decodeLine1ZP),y
			lda _tmpChr+3
			sta (decodeLine2ZP),y
				
		; Move to next col
		ldy _curCol
		iny		
		jmp loopCols	

	doneCols:		
		
		; Update address of location in charmap
		clc	
		lda charmapDataZP			
		adc _charmapWidth
		sta charmapDataZP	
		bcc nocarryCM	; Check if carry to high byte
		inc charmapDataZP+1
	nocarryCM:

		; Update address of location of screen buffer (line 1)
		clc	
		lda decodeLine1ZP			
		adc _blockWidth
		sta decodeLine1ZP	
		bcc nocarrySB1	; Check if carry to high byte
		inc decodeLine1ZP+1
	nocarrySB1:

		; Update address of location of screen buffer (line 2)
		clc	
		lda decodeLine2ZP		
		adc _blockWidth
		sta decodeLine2ZP
		bcc nocarrySB2	; Check if carry to high byte
		inc decodeLine2ZP+1
	nocarrySB2:
	
	; Move to next row
	jmp loopRows

doneRows:
	rts

.endproc	
