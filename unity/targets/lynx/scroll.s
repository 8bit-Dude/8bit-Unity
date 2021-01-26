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

	.export _Scroll
	
	.import _screenWidth, _screenHeight
	.import _blockWidth, _charsetData
	
charPointerZP = $b5
scrPointerZP  = $b7
charBlockZP	  = $bb
	
	.segment	"BSS"

_tmpY: .res 1
_tmpChr: .res 1
_curCol: .res 1
_curRow: .res 1

	.segment	"CODE"		
	
; ---------------------------------------------------------------
; void __near__ _Scroll (void)
;	Blit page from Charmap to Bitmap
;	Zero Page Data:
;		charPointerZP: 16 bit address of location on charmap (auto-updated)
;		scrPointerZP:  16 bit address of location on screen (auto-updated)
;
;		charBlockZP:   16 bit address of curent charset block (auto-generated)
; ---------------------------------------------------------------	

.proc _Scroll: near

	lda #0
	sta _curRow
	
loopRows: 
	lda _curRow
	cmp _screenHeight
	bpl doneRows
	inc _curRow
	
		; Reset Char Block Addr
		lda _charsetData
		sta  charBlockZP
		lda _charsetData+1
		sta  charBlockZP+1
				
		ldx #0
	loopLines:
		cpx #6
		beq doneLines	
		inx 
		
			ldy #0
		loopCols:
			cpy _screenWidth
			bpl doneCols			
			sty _curCol
		
				; Get Char Value
				lda (charPointerZP),y
				
				; Set Chr Offset
				asl A			; Multiply by 2	(2 bytes per col!)
				sta _tmpChr
				
				; Set Col Offset
				tya
				asl A			; Multiply by 2	(2 bytes per col!)
				sta _tmpY
				
				; Copy 1st byte
				ldy _tmpChr
				lda (charBlockZP),y		
				ldy _tmpY				
				sta (scrPointerZP),y		

				; Copy 2nd byte
				ldy _tmpChr
				iny 
				lda (charBlockZP),y		
				ldy _tmpY				
				iny 
				sta (scrPointerZP),y			

			; Move to next col
			ldy _curCol
			iny
			jmp loopCols	

		doneCols:

			; Update address of charblock (+256)
			inc charBlockZP+1
		
			; Update address of screen (+82)
			clc	
			lda scrPointerZP
			adc #82
			sta scrPointerZP
			bcc nocarryScrPtr	; Check if carry to high byte
			inc scrPointerZP+1
		nocarryScrPtr:		
		
		; Move to next line
		jmp loopLines
	
	doneLines:		
		
		; Update address of location in charmap
		clc	
		lda charPointerZP			
		adc _blockWidth
		sta charPointerZP
		bcc nocarryChrPtr	; Check if carry to high byte
		inc charPointerZP+1
	nocarryChrPtr:
	
	; Move to next row
	jmp loopRows

doneRows:
	rts

.endproc
