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

	.export _BlitCharmap

	.import _hiresLinesHI, _hiresLinesLO
	.import _screenCol1, _screenWidth
	.import _screenRow1, _screenRow2
	.import _charsetData, _blockWidth
	
; Zeropage addresses	
mainAuxTog = $42
currColZP  = $e3
charPtrZP  = $ce
scrPtrZP   = $ef
charLPtrZP = $fb 
charRPtrZP = $fd
	
	.segment	"BSS"	
	
_curRow: .res 1
_curLine: .res 1
	
	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ BlitCharmap (void)
;	Blit data from Charmap to Screen
;	Zero Page Data:
;		charPtrZP:  16 bit address of location on charmap (auto-updated)
;		scrPtrZP:   16 bit address of current Hires line (auto-generated)
;		charLPtrZP: 16 bit address of charset block L (auto-generated)
;		charRPtrZP: 16 bit address of charset block R (auto-generated)
; ---------------------------------------------------------------	

_BlitCharmap:

.ifdef __DHR__
	; Init Main/Aux Toggle
	lda #0
	sta mainAuxTog  
.endif

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
	
		; Copy 8 lines (1 row of chars)
		jsr copyLines
	
	; Move to next row
	jmp loopRows

doneRows:
	rts


; ---------------------------------------------------------------	
copyLines:
	; Set Addresses of L/R charsets
	lda _charsetData
	sta  charLPtrZP
	sta  charRPtrZP
	lda _charsetData+1
	sta  charLPtrZP+1
	clc
  .ifdef __DHR__
	adc #08
  .else	
	adc #04
  .endif	
	sta  charRPtrZP+1

	ldx #0
loopLines:
	cpx #8
	beq doneLines	
	inx
	
		; Set address of start pixel on Hires line
		ldy _curLine
		lda _hiresLinesHI,y
		sta  scrPtrZP+1
		lda _hiresLinesLO,y
		clc
		adc _screenCol1
		sta  scrPtrZP
		inc _curLine
	
	.ifdef __DHR__
		; Main/Aux Toggle
	branchAux:	
		sta $c055		; Switch to AUX memory
		jmp switchDone
	branchMain:
		sta $c054		; Switch to MAIN memory
	switchDone:			
	.endif	
	
		ldy #0
	loopCols:
		cpy _screenWidth
		bpl doneCols

			; Copy Left Char
			lda (charPtrZP),y		; Get char value
			sty currColZP		
			tay 
			lda (charLPtrZP),y		; Get L pixels for that char
			ldy currColZP
			sta (scrPtrZP),y		; Save in Hires mem
			iny						; Move to next col

			; Copy Right Char
			lda (charPtrZP),y		; Get char value
			sty currColZP
			tay 
			lda (charRPtrZP),y		; Get R pixels for that char
			ldy currColZP
			sta (scrPtrZP),y		; Save in Hires mem
			iny						; Move to next col
			
		jmp loopCols

	doneCols:

		; Update address of charset block L (+128)
		lda charLPtrZP			
		clc	
		adc #128
		sta charLPtrZP	
		bcc nocarryBlockLPtr	; Check if carry to high byte
		inc charLPtrZP+1
	nocarryBlockLPtr:

		; Update address of charset block R (+128)
		lda charRPtrZP
		clc	
		adc #128
		sta charRPtrZP	
		bcc nocarryBlockRPtr	; Check if carry to high byte
		inc charRPtrZP+1
	nocarryBlockRPtr:
	
	.ifdef __DHR__
		; Toggle AUX/MAIN
		lda mainAuxTog
		eor #1
		sta mainAuxTog
		bne branchMain			
	.endif	
		
	; Move to next line
	jmp loopLines

doneLines:

	; Update address of location in charmap
	lda charPtrZP			
	clc	
	adc _blockWidth
	sta charPtrZP	
	bcc nocarryChrPtr		; Check if carry to high byte
	inc charPtrZP+1
nocarryChrPtr:

	rts
