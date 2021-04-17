;
; Copyright (c) 2021 Anthony Beaucamp.
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
	
	.import _scrollCols, _scrollRows, _scrollDirX, _scrollDirY
	.import _hiresLinesHI, _hiresLinesLO	

; Zeropage addresses
mainAuxTog = $42  
scrRow1ZP  = $fb
scrCol1ZP  = $fc
scrRow2ZP  = $fd
scrCol2ZP  = $fe
scrPtr1ZP  = $eb
scrPtr2ZP  = $ed

	.segment	"CODE"	

; ---------------------------------------------------------------
; void __near__ _Scroll (void)
;	Zero Page Data:
;		scrRow1ZP, scrCol1ZP: Coordinates of screen source      (auto-updated)
;		scrRow2ZP, scrCol2ZP: Coordinates of screen destination (auto-updated)
;		scrPtr1ZP: 16 bit address of screen source      (auto-generated)
;		scrPtr2ZP: 16 bit address of screen destination (auto-generated)
; ---------------------------------------------------------------	

_Scroll:

.ifdef __DHR__
	; Init Main/Aux Toggle
	lda #0
	sta mainAuxTog  
.endif

	ldx #0
loopRows: 
	cpx _scrollRows
	beq doneRows

		jsr copyRow
	
	inx	
	jmp loopRows		
	
doneRows:
	rts


; ----------------------------------
copyRow:
	; Get address of source
	ldy scrRow1ZP
	lda _hiresLinesHI,y
	sta  scrPtr1ZP+1
	lda _hiresLinesLO,y
	clc
	adc  scrCol1ZP
	sta  scrPtr1ZP		

	; Get address of destination
	ldy scrRow2ZP
	lda _hiresLinesHI,y
	sta  scrPtr2ZP+1
	lda _hiresLinesLO,y
	clc
	adc  scrCol2ZP
	sta  scrPtr2ZP		

.ifdef __DHR__
	; Main/Aux Toggle
branchAux:	
	sta $c055		; Switch to AUX memory
	jmp switchDone
branchMain:
	sta $c054		; Switch to MAIN memory
switchDone:			
.endif	

	; Check horz scanning direction
	lda _scrollDirX
	cmp #1
	bne scrollLeft
	
	;-------------------------------
	scrollRight:
		ldy #0
	loopColsRight:
		cpy _scrollCols
		beq doneCols
		
			; Copy 1 screen byte
			lda (scrPtr1ZP),y
			sta (scrPtr2ZP),y

		; Move to next col
		iny		
		jmp loopColsRight
		
	;-------------------------------
	scrollLeft:
		ldy _scrollCols
	loopColsLeft:
		dey		
		
			; Copy 1 byte
			lda (scrPtr1ZP),y
			sta (scrPtr2ZP),y	

		; Move to next col
		cpy #0
		beq doneCols
		jmp loopColsLeft		
		
doneCols:
.ifdef __DHR__
	; Toggle AUX/MAIN
	lda mainAuxTog
	eor #1
	sta mainAuxTog
	bne branchMain			
.endif
		
	; Check vert scanning direction
	lda _scrollDirY
	cmp #1
	bne scrollDown

	;-------------------------------
	scrollUp:		
		inc scrRow1ZP
		inc scrRow2ZP
		jmp doneCopy

	;-------------------------------
	scrollDown:
		dec scrRow1ZP
		dec scrRow2ZP
	
doneCopy:
	rts
