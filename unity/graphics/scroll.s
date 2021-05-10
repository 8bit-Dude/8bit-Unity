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

; Zeropage addresses

.ifdef __ATARI__
	scrPtr1ZP = $e4
	scrPtr2ZP = $e6
	lineWidth = $28	; 40
.endif	
	
.ifdef __CBM__
	scrPtr1ZP = $61
	scrPtr2ZP = $63
	clrPtr1ZP = $fb
	clrPtr2ZP = $fd
	lineWidth = $28	; 40
.endif	

.ifdef __LYNX__
	scrPtr1ZP = $b7
	scrPtr2ZP = $b9
	lineWidth = $52	; 82
.endif	

.ifdef __NES__
	scrPtr1ZP = $00
	scrPtr2ZP = $00
	lineWidth = $20	; 32
.endif

.ifdef __ATMOS__
	scrPtr1ZP = $b4
	scrPtr2ZP = $b6
	lineWidth = $28	; 40
.endif

	.segment	"CODE"	

; ---------------------------------------------------------------
; void __near__ _Scroll (void)
;	Zero Page Data:
;		scrPtr1ZP: 16 bit address of screen source      (auto-updated)
;		scrPtr2ZP: 16 bit address of screen destination (auto-updated)
; (CBM) clrPtr1ZP: 16 bit address of color source       (auto-updated)
; (CBM) clrPtr2ZP: 16 bit address of color destination  (auto-updated)
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
			lda (scrPtr1ZP),y
			sta (scrPtr2ZP),y
		.ifdef __CBM__
			; Copy 1 color byte
			lda (clrPtr1ZP),y
			sta (clrPtr2ZP),y
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
			lda (scrPtr1ZP),y
			sta (scrPtr2ZP),y
		.ifdef __CBM__
			; Copy 1 color byte
			lda (clrPtr1ZP),y
			sta (clrPtr2ZP),y
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
		lda clrPtr1ZP			
		clc	
		adc #lineWidth
		sta clrPtr1ZP	
		bcc nocarryClrPtr1	; Check if carry to high byte
		inc clrPtr1ZP+1
	nocarryClrPtr1:

		lda clrPtr2ZP
		clc	
		adc #lineWidth
		sta clrPtr2ZP
		bcc nocarryClrPtr2	; Check if carry to high byte
		inc clrPtr2ZP+1
	nocarryClrPtr2:
	.endif	
	
		lda scrPtr1ZP			
		clc	
		adc #lineWidth
		sta scrPtr1ZP	
		bcc nocarryScrPtr1	; Check if carry to high byte
		inc scrPtr1ZP+1
	nocarryScrPtr1:

		lda scrPtr2ZP		
		clc	
		adc #lineWidth
		sta scrPtr2ZP
		bcc nocarryScrPtr2	; Check if carry to high byte
		inc scrPtr2ZP+1
	nocarryScrPtr2:	
		jmp doneCopy

	;-------------------------------
	scrollDown:
	
	.ifdef __CBM__
		lda clrPtr1ZP			
		sec	
		sbc #lineWidth
		sta clrPtr1ZP	
		bcs carryClrPtr1	; Check if carry to high byte
		dec clrPtr1ZP+1
	carryClrPtr1:

		lda clrPtr2ZP		
		sec	
		sbc #lineWidth
		sta clrPtr2ZP
		bcs carryClrPtr2	; Check if carry to high byte
		dec clrPtr2ZP+1
	carryClrPtr2:
	.endif	
	
		lda scrPtr1ZP			
		sec	
		sbc #lineWidth
		sta scrPtr1ZP	
		bcs carryScrPtr1	; Check if carry to high byte
		dec scrPtr1ZP+1
	carryScrPtr1:

		lda scrPtr2ZP		
		sec	
		sbc #lineWidth
		sta scrPtr2ZP
		bcs carryScrPtr2	; Check if carry to high byte
		dec scrPtr2ZP+1
	carryScrPtr2:
	
doneCopy:
	rts
