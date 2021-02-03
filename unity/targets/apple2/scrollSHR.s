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

	.export _ScrollSHR

	.import _hiresLinesHI, _hiresLinesLO
	.import _screenCol1, _screenWidth
	.import _screenRow1, _screenRow2
	.import _charsetData, _blockWidth
	
charPointerZP  = $ce
scrPointerZP   = $ef
charLPointerZP = $fb 
charRPointerZP = $fd
	
	.segment	"BSS"	
	
_tmpY: .res 1
_curRow: .res 1
_curLine: .res 1
	
	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _ScrollSHR (void)
;	Blit data from Charmap to Bitmap
;	Zero Page Data:
;		charPointerZP:  16 bit address of location on charmap (auto-updated)
;
;		scrPointerZP:   16 bit address of current Hires line (auto-generated)
;		charLPointerZP: 16 bit address of charset block L (auto-generated)
;		charRPointerZP: 16 bit address of charset block R (auto-generated)
; ---------------------------------------------------------------	

.proc _ScrollSHR: near

; Set start row/line
	lda _screenRow1
	sta _curRow
	asl A
	asl A
	asl A
	sta _curLine

loopRows: 
	lda _curRow
	cmp _screenRow2
	bpl doneRows
	inc _curRow
	
	; Set Charset Addresses (L/R)
	lda _charsetData
	sta  charLPointerZP
	sta $fd
	
	lda _charsetData+1
	sta  charLPointerZP+1
	adc #04
	sta $fe

		ldx #0
	loopLines:
		cpx #8
		beq doneLines	
		inx
		
		; Set address of start pixel on Hires line
		ldy _curLine
		lda _hiresLinesHI,y
		sta  scrPointerZP+1
		lda _hiresLinesLO,y
		adc _screenCol1
		sta  scrPointerZP
		inc _curLine
		
			ldy #0
		loopCols:
			cpy _screenWidth
			bpl doneCols

				;---------------------
				; Handle Left Char
				lda (charPointerZP),y		; Get char value
				sty _tmpY		
				tay 
				lda (charLPointerZP),y		; Get L pixels for that char
				ldy _tmpY
				sta (scrPointerZP),y		; Save in Hires mem

				; Move to next col
				iny

				;---------------------
				; Handle Right Char
				lda (charPointerZP),y		; Get char value
				
				sty _tmpY
				tay 
				lda (charRPointerZP),y		; Get R pixels for that char
				ldy _tmpY
				sta (scrPointerZP),y		; Save in Hires mem

				; Move to next col
				iny
				
			jmp loopCols

		doneCols:

			; Update address of charset block L (+128)
			clc	
			lda charLPointerZP			
			adc #128
			sta charLPointerZP	
			bcc nocarryBlockLPtr	; Check if carry to high byte
			inc charLPointerZP+1
		nocarryBlockLPtr:

			; Update address of charset block R (+128)
			clc	
			lda $fd			
			adc #128
			sta $fd	
			bcc nocarryBlockRPtr	; Check if carry to high byte
			inc $fe
		nocarryBlockRPtr:
			
		; Move to next line
		jmp loopLines
	
	doneLines:

		; Update address of location in charmap
		clc	
		lda charPointerZP			
		adc _blockWidth
		sta charPointerZP	
		bcc nocarryChrPtr		; Check if carry to high byte
		inc charPointerZP+1
	nocarryChrPtr:
	
	; Move to next row
	jmp loopRows

doneRows:
	rts

.endproc
