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
	
	.import _scrollCols, _scrollRows, _scrollDirX, _scrollDirY, _scrollLine

; Zeropage addresses

.ifdef __APPLE2__
	scr1PointerZP = $fb
	scr2PointerZP = $fd
.endif

.ifdef __ATARI__
	scr1PointerZP = $e4
	scr2PointerZP = $e6
.endif	
	
.ifdef __CBM__
	scr1PointerZP = $61
	scr2PointerZP = $63
	clr1PointerZP = $fb
	clr2PointerZP = $fd
.endif	

.ifdef __LYNX__
	scr1PointerZP = $b7
	scr2PointerZP = $b9
.endif	

.ifdef __ATMOS__
	scr1PointerZP = $b4
	scr2PointerZP = $b6
.endif

	.segment	"CODE"	

; ---------------------------------------------------------------
; void __near__ _Scroll (void)
;	Zero Page Data:
;		scr1PointerZP: 16 bit address of screen source      (auto-updated)
;		scr2PointerZP: 16 bit address of screen destination (auto-updated)
; (CBM) clr1PointerZP: 16 bit address of color source       (auto-updated)
; (CBM) clr2PointerZP: 16 bit address of color destination  (auto-updated)
; ---------------------------------------------------------------	

_Scroll:

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
			lda (scr1PointerZP),y
			sta (scr2PointerZP),y
		.ifdef __CBM__
			; Copy 1 color byte
			lda (clr1PointerZP),y
			sta (clr2PointerZP),y
		.endif	

		; Move to next col
		iny		
		jmp loopColsRight
		
	;-------------------------------
	scrollLeft:
		ldy _scrollCols
	loopColsLeft:
		dey		
		
			; Copy 1 byte
			lda (scr1PointerZP),y
			sta (scr2PointerZP),y
		.ifdef __CBM__
			; Copy 1 color byte
			lda (clr1PointerZP),y
			sta (clr2PointerZP),y
		.endif	

	; Move to next col
	cpy #0
	beq doneCols
	jmp loopColsLeft		
		
doneCols:		
	; Check vert scanning direction
	lda _scrollDirY
	cmp #1
	bne scrollDown

	;-------------------------------
	scrollUp:	
	
	.ifdef __CBM__
		clc	
		lda clr1PointerZP			
		adc _scrollLine
		sta clr1PointerZP	
		bcc nocarryClrPtr1	; Check if carry to high byte
		inc clr1PointerZP+1
	nocarryClrPtr1:

		clc	
		lda clr2PointerZP
		adc _scrollLine
		sta clr2PointerZP
		bcc nocarryClrPtr2	; Check if carry to high byte
		inc clr2PointerZP+1
	nocarryClrPtr2:
	.endif	
	
		clc	
		lda scr1PointerZP			
		adc _scrollLine
		sta scr1PointerZP	
		bcc nocarryScrPtr1	; Check if carry to high byte
		inc scr1PointerZP+1
	nocarryScrPtr1:

		clc	
		lda scr2PointerZP		
		adc _scrollLine
		sta scr2PointerZP
		bcc nocarryScrPtr2	; Check if carry to high byte
		inc scr2PointerZP+1
	nocarryScrPtr2:	
		jmp doneCopy

	;-------------------------------
	scrollDown:
	
	.ifdef __CBM__
		sec	
		lda clr1PointerZP			
		sbc _scrollLine
		sta clr1PointerZP	
		bcs carryClrPtr1	; Check if carry to high byte
		dec clr1PointerZP+1
	carryClrPtr1:

		sec	
		lda clr2PointerZP		
		sbc _scrollLine
		sta clr2PointerZP
		bcs carryClrPtr2	; Check if carry to high byte
		dec clr2PointerZP+1
	carryClrPtr2:
	.endif	
	
		sec	
		lda scr1PointerZP			
		sbc _scrollLine
		sta scr1PointerZP	
		bcs carryScrPtr1	; Check if carry to high byte
		dec scr1PointerZP+1
	carryScrPtr1:

		sec	
		lda scr2PointerZP		
		sbc _scrollLine
		sta scr2PointerZP
		bcs carryScrPtr2	; Check if carry to high byte
		dec scr2PointerZP+1
	carryScrPtr2:
	
doneCopy:
	rts
	
	
