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

	.export _ScrollDHR

	.import _hiresLinesHI, _hiresLinesLO
	.import _screenCol1, _screenCol2
	.import _screenRow1, _screenRow2
	.import _charsetData, _charmapWidth	
		
	.segment	"BSS"	
	
_tmp: .res 1
_curRow: .res 1
_curLine: .res 1
_mainAuxTog: .res 1
	
	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _ScrollDHR (void)
;	Blit data from Charmap to Bitmap
;	Zero Page Data:
;		$ef: 16 bit address of location on charmap (auto-updated)
;
;		$ce: 16 bit address of current Hires line (auto-generated)
;		$fb: 16 bit address of charset block L (auto-generated)
;		$fd: 16 bit address of charset block R (auto-generated)
; ---------------------------------------------------------------	

_ScrollDHR:

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
	sta $fb
	sta $fd
	
	lda _charsetData+1
	sta $fc
	adc #08
	sta $fe

		ldx #0
	loopLines:
		cpx #8
		beq doneLines	
		inx
		
			; Set Hires address of current line
			ldy _curLine
			lda _hiresLinesHI,y
			sta $cf
			lda _hiresLinesLO,y
			sta $ce
			inc _curLine		
			
			;---------------------
			; Main/Aux Toggle
		branchAux:	
			sta $c055		; Switch to AUX memory
			jmp switchDone
		branchMain:
			sta $c054		; Switch to MAIN memory
		switchDone:			
		
			;---------------------
			; Copy 1 line
			jsr copyLine

			;---------------------
			; Update address of charset block L (+128)
			clc	
			lda $fb			
			adc #128
			sta $fb	
			bcc nocarryCL	; Check if carry to high byte
			inc $fc
		nocarryCL:

			;---------------------
			; Update address of charset block R (+128)
			clc	
			lda $fd			
			adc #128
			sta $fd	
			bcc nocarryCR	; Check if carry to high byte
			inc $fe
		nocarryCR:
			
			;---------------------
			; Toggle AUX/MAIN
			clc
			lda _mainAuxTog
			eor #1
			sta _mainAuxTog
			bne branchMain			
			
		; Move to next line
		jmp loopLines
	
	doneLines:

		; Update address of charmap line
		clc	
		lda $ef			
		adc _charmapWidth
		sta $ef	
		bcc nocarryCM	; Check if carry to high byte
		inc $f0
	nocarryCM:
	
	; Move to next row
	jmp loopRows

doneRows:
	rts


;----------------------------------
; Copy 1 line of data (AUX or MAIN)
copyLine:
	ldy _screenCol1
loopCols:
	cpy _screenCol2			; Number of cols
	bpl doneCols
	
		;---------------------
		; Handle Left Char
		lda ($ef),y		; Get char value
		sty _tmp		
		tay 
		lda ($fb),y		; Get L pixels for that char
		ldy _tmp
		sta ($ce),y		; Save in Hires mem

		; Move to next col
		iny

		;---------------------
		; Handle Right Char
		lda ($ef),y		; Get char value
		
		sty _tmp
		tay 
		lda ($fd),y		; Get R pixels for that char
		ldy _tmp
		sta ($ce),y		; Save in Hires mem

		; Move to next col
		iny
		
	jmp loopCols

doneCols:
	rts
	
