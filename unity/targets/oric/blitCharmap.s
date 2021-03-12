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

	.import _screenHeight, _screenWidth
	.import _charsetData, _blockWidth
	
charPointerZP = $b5
scrPointerZP  = $b7
charBlockZP	  = $b9
	
	.segment	"BSS"	
	
_tmp: .res 1
_curRow: .res 1
	
	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ BlitCharmap (void)
;	Blit data from Charmap to Screen
;	Zero Page Data:
;		charPointerZP: 16 bit address of location on charmap (auto-updated)
;		scrPointerZP:  16 bit address of location on screen (auto-updated)
;
;		charBlockZP:   16 bit address of curent charset block (auto-generated)
; ---------------------------------------------------------------	

.proc _BlitCharmap: near

	lda #0
	sta _curRow
	
loopRows: 
	lda _curRow
	cmp _screenHeight
	bpl doneRows
	inc _curRow
	
	; Reset Charset Block Addr
	lda _charsetData
	sta  charBlockZP
	lda _charsetData+1
	sta  charBlockZP+1

		ldx #0
	loopLines:
		cpx #8
		beq doneLines	
		inx 
		
			ldy #0
		loopCols:
			cpy _screenWidth
			bpl doneCols

				; Copy Char
				lda (charPointerZP),y		; Get char value
				sty _tmp
				tay 
				lda (charBlockZP),y			; Get pixels for that char
				ldy _tmp
				sta (scrPointerZP),y		; Save in Hires mem	

				; Move to next col
				iny
				
			jmp loopCols

		doneCols:

			; Update address of charset block (+128)
			clc	
			lda charBlockZP
			adc #128
			sta charBlockZP
			bcc nocarryBlcPtr	; Check if carry to high byte
			inc charBlockZP+1
		nocarryBlcPtr:
			
			; Update address of Hires line (+40)
			clc	
			lda scrPointerZP
			adc #40
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
