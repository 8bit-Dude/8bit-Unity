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
	.import _blockWidth, _lineWidth
	
charPointerZP = $e2
scrPointerZP  = $e4
	
	.segment	"BSS"

_tmpChr: .res 1
_curCol: .res 1
_curRow: .res 1

	.segment	"CODE"		
	
; ---------------------------------------------------------------
; void __near__ BlitCharmap (void)
;	Blit page from Charmap to Screen
;	Zero Page Data:
;		charPointerZP: 16 bit address of location on charmap (auto-updated)
;		scrPointerZP:  16 bit address of location on screen (auto-updated)
; ---------------------------------------------------------------	

.proc _BlitCharmap: near

	lda _screenHeight
	sta _curRow
loopRows: 
							
		ldy #0
	loopCols:
	
			; Transfer Char value to screen
			lda (charPointerZP),y
			sta (scrPointerZP),y

		; Move to next col
		iny
		cpy _screenWidth
		bcc loopCols

	doneCols:
		; Update address of location in screen RAM
		clc	
		lda scrPointerZP
		adc _lineWidth
		sta scrPointerZP
		bcc nocarryScrPtr	; Check if carry to high byte
		inc scrPointerZP+1
	nocarryScrPtr:		
		
		; Update address of location in charmap
		clc	
		lda charPointerZP			
		adc _blockWidth
		sta charPointerZP
		bcc nocarryChrPtr	; Check if carry to high byte
		inc charPointerZP+1
	nocarryChrPtr:
	
	; Move to next row
	dec _curRow
	bne loopRows

doneRows:
	rts

.endproc