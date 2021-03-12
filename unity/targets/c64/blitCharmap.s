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
	
	.import _screenWidth, _screenHeight
	.import _blockWidth
	
charattDataZP = $61
charPointerZP = $63
scrPointerZP  = $fb
colPointerZP  = $fd
	
	.segment	"BSS"

_curCol: .res 1
_curRow: .res 1

	.segment	"CODE"		
	
; ---------------------------------------------------------------
; void __near__ BlitCharmap (void)
;	Blit page from Charmap to Screen
;	Zero Page Data:
;		charattDataZP: 16 bit address of char attribute data (auto-updated)
;		charPointerZP: 16 bit address of location on charmap (auto-updated)
;		scrPointerZP:  16 bit address of location on screen (auto-updated)
; ---------------------------------------------------------------	

.proc _BlitCharmap: near

	lda #0
	sta _curRow
	
loopRows: 
	lda _curRow
	cmp _screenHeight
	bpl doneRows
	inc _curRow
							
		ldy #0
	loopCols:
		cpy _screenWidth
		bpl doneCols			
		sty _curCol
	
			; Get Char Value and save to ScreenRAM
			lda (charPointerZP),y
			sta (scrPointerZP),y		
			
			; Get  Atrribute and save to ColorRAM
			tay
			lda (charattDataZP),y
			ldy _curCol
			sta (colPointerZP),y								

		; Move to next col
		iny
		jmp loopCols	

	doneCols:

		; Update address of screen (+40)
		clc	
		lda scrPointerZP
		adc #40
		sta scrPointerZP
		bcc nocarryScrPtr	; Check if carry to high byte
		inc scrPointerZP+1
	nocarryScrPtr:		
		
		; Update address of color (+40)
		clc	
		lda colPointerZP
		adc #40
		sta colPointerZP
		bcc nocarryColPtr	; Check if carry to high byte
		inc colPointerZP+1
	nocarryColPtr:
	
		; Update location in charmap
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