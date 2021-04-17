;
; Copyright (c) 2019 Anthony Beaucamp.
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

	.export _BlitSprite

	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ BlitSprite (void)
;	Copy a series of evenly spaced blocks
;	Zero Page Data:
;		$b0: Number of blocks
;		$b1: Number of bytes per block
;		$b2: 16 bit address of 1st source block
;		$b4: 16 bit address of 1st target block
;		$b6: Offset between source blocks
;		$b7: Offset between target blocks
; ---------------------------------------------------------------	

.proc _BlitSprite: near
	ldx $b0			; X loop: Number of blocks (ZP$b0)
loopx: 
	ldy $b1			; Y loop: Number of bytes per block (ZP$b1)
loopy:	
	lda ($b2),y		; Copy 1 byte from source (ZP$b2,y) to target (ZP$b4,y)
	sta ($b4),y
	dey				; Iterate Y loop
	bne loopy
	
	clc
	lda $b2			; Update address of next source block
	adc $b6			; Add block offset (ZP$b6)
	sta $b2	
	bcc nocarry1	; Check if carry to high byte
	inc $b3
nocarry1:

	clc
	lda $b4			; Update address of next target block
	adc $b7			; Add block offset (ZP$b7)
	sta $b4	
	bcc nocarry2	; Check if carry to higher byte
	inc $b5
nocarry2:

	dex				; Iterate X loop
	bne loopx
	rts
.endproc
