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

	.export _MultiBlockCopy

	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _MultiBlockCopy (void)
;	Copy a series of evenly spaced blocks
;	Zero Page Data:
;		$00: Number of blocks
;		$01: Number of bytes per block
;		$02: 16 bit address of 1st source block
;		$04: 16 bit address of 1st target block
;		$06: Offset between source blocks
;		$07: Offset between target blocks
; ---------------------------------------------------------------	

.proc _MultiBlockCopy: near
	ldx $00			; X loop: Number of blocks (ZP$00)
loopx: 
	ldy $01			; Y loop: Number of bytes per block (ZP$01)
loopy:	
	lda ($02),y		; Copy 1 byte from source (ZP$02,y) to target (ZP$04,y)
	sta ($04),y
	dey				; Iterate Y loop
	bne loopy
	clc
	lda $02			; Set address of next source block
	adc $06			; Add block offset (ZP$06)
	sta $02	
	bcc nocarry1	; Check if carry to high byte
	inc $03
nocarry1:
	clc
	lda $04			; Set address of next target block
	adc $07			; Add block offset (ZP$07)
	sta $04	
	bcc nocarry2	; Check if carry to higher byte
	inc $05
nocarry2:
	dex				; Iterate X loop
	bne loopx
.endproc
