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

	.import _screenCol1, _screenCol2
	.import _screenRow1, _screenRow2
	.import _charsetData, _charmapWidth
	
	.segment	"BSS"	
	
_tmp: .res 1
_curRow: .res 1
	
	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _Scroll (void)
;	Blit data from Charmap to Bitmap
;	Zero Page Data:
;		$ef: 16 bit address of location on charmap (auto-updated)
;		$b7: 16 bit address of current Hires line (auto-updated)
;
;		$b9: 16 bit address of curent charset block (auto-generated)
; ---------------------------------------------------------------	

.proc _Scroll: near

	lda _screenRow1
	sta _curRow
	
loopRows: 
	lda _curRow
	cmp _screenRow2
	bpl doneRows
	inc _curRow
	
	; Reset Charset Block Addr
	lda _charsetData
	sta $b9
	lda _charsetData+1
	sta $ba		

		ldx #0
	loopLines:
		cpx #8
		beq doneLines	
		inx 
		
			ldy _screenCol1
		loopCols:
			cpy _screenCol2
			bpl doneCols

				; Copy Char
				lda ($b5),y		; Get char value
				sty _tmp
				tay 
				lda ($b9),y		; Get pixels for that char
				ldy _tmp
				sta ($b7),y		; Save in Hires mem	

				; Move to next col
				iny
				
			jmp loopCols

		doneCols:

			; Update address of charset block (+128)
			clc	
			lda $b9			; Update address of charset block
			adc #128
			sta $b9	
			bcc nocarryCS	; Check if carry to high byte
			inc $ba
		nocarryCS:
			
			; Update address of Hires line (+40)
			clc	
			lda $b7			; Update address of Hires line
			adc #40
			sta $b7
			bcc nocarryBM	; Check if carry to high byte
			inc $b8
		nocarryBM:
			
		; Move to next line
		jmp loopLines
	
	doneLines:

		; Update address of location in charmap
		clc	
		lda $b5			
		adc _charmapWidth
		sta $b5	
		bcc nocarryCM	; Check if carry to high byte
		inc $b6
	nocarryCM:
	
	; Move to next row
	jmp loopRows

doneRows:
	rts

.endproc
