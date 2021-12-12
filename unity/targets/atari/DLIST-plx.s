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

	.export _ParallaxDLIST

; Work variable
_addr: .res 2

	.segment	"CODE"

; ---------------------------------------------------------------
; void __near__ ParallaxDLIST (void)
; ---------------------------------------------------------------	

.proc _ParallaxDLIST: near
	
header:
	lda #$70		
	sta $0900
	sta $0901
	lda #$f0	
	sta $0902
	
setupPlxList:		
	lda #$70
	sta _addr+0
	lda #$09
	sta _addr+1
	
	ldy #0
	ldx #0
loopCHR:
	lda #$d4		; DLI + Address + Text Mode
	sta $0903,x
	inx
	lda _addr+0
	sta $0903,x
	inx
	lda _addr+1
	sta $0903,x
	inx
	
	lda _addr+0		
	clc	
	adc #44
	sta _addr+0	
	bcc nocarry
	inc _addr+1
nocarry:
	
	iny
	cpy #25
	bcc loopCHR
	
footer:	
	lda #$41		; Footer
	sta $0903,x
	inx

	lda #$20
	sta $0903,x
	inx

	lda #$09
	sta $0903,x
	
	lda #$00		; Lower addres of DLIST $0920 -> $0900
	sta $0230	
	lda #$09
	sta $0231	
	rts
.endproc
