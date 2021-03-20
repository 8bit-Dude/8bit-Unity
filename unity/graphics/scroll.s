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
	row1PointerZP = $fb
	row2PointerZP = $fd
.endif

.ifdef __ATARI__
	row1PointerZP = $e4
	row2PointerZP = $e6
.endif	
	
.ifdef __CBM__
	row1PointerZP = $fb
	row2PointerZP = $fd
.endif	

.ifdef __LYNX__
	row1PointerZP = $b7
	row2PointerZP = $b9
.endif	

.ifdef __ATMOS__
	row1PointerZP = $b7
	row2PointerZP = $b9
.endif

	.segment	"CODE"	

; ---------------------------------------------------------------
; void __near__ _Scroll (void)
;	Zero Page Data:
;		row1PointerZP: 16 bit address of source (auto-updated)
;		row2PointerZP: 16 bit address of destination (auto-updated)
; ---------------------------------------------------------------	

.proc _Scroll: near

	ldx #0
loopRows: 
	cpx _scrollRows
	beq doneRows

	;-------------------------------
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
		
			; Copy 1 byte
			lda (row1PointerZP),y
			sta (row2PointerZP),y

		; Move to next col
		iny		
		jmp loopColsRight
		
	;-------------------------------
	scrollLeft:
		ldy _scrollCols
	loopColsLeft:
		dey		
		
			; Copy 1 byte
			lda (row1PointerZP),y
			sta (row2PointerZP),y

		; Move to next col
		cpy #0
		beq doneCols
		jmp loopColsLeft		
		
	doneCols:		

	;-------------------------------
	; Check vert scanning direction
		lda _scrollDirY
		cmp #1
		bne scrollDown

	;-------------------------------
	scrollUp:
		clc	
		lda row1PointerZP			
		adc _scrollLine
		sta row1PointerZP	
		bcc nocarryScrPtr1	; Check if carry to high byte
		inc row1PointerZP+1
	nocarryScrPtr1:

		clc	
		lda row2PointerZP		
		adc _scrollLine
		sta row2PointerZP
		bcc nocarryScrPtr2	; Check if carry to high byte
		inc row2PointerZP+1
	nocarryScrPtr2:
		jmp carryScrPtr2

	;-------------------------------
	scrollDown:
		sec	
		lda row1PointerZP			
		sbc _scrollLine
		sta row1PointerZP	
		bcs carryScrPtr1	; Check if carry to high byte
		dec row1PointerZP+1
	carryScrPtr1:

		sec	
		lda row2PointerZP		
		sbc _scrollLine
		sta row2PointerZP
		bcs carryScrPtr2	; Check if carry to high byte
		dec row2PointerZP+1
	carryScrPtr2:
	
	;-------------------------------
	; Move to next row
	inx	
	jmp loopRows		
	
doneRows:
	rts

.endproc		
