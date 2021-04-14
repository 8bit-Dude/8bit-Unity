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

	.export _BitmapDLIST

	.segment	"CODE"

; ---------------------------------------------------------------
; void __near__ _BitmapDLIST (void)
; ---------------------------------------------------------------	

.proc _BitmapDLIST: near
; Raise addres of DLIST $0900 -> $0920
	lda #$20		
	sta $0230	
	
; Header
	lda #$70		
	sta $0920
	sta $0921
	sta $0922
	
; Body
	lda #$ce	; Video Address + DLI (x1)
	sta $0923
	lda #$10
	sta $0924
	lda #$70
	sta $0925

; 12 groups of 7xHires lines + 1xDLI
	ldx #0
loop1:
	
	ldy #0
	lda #$0e	; Hires lines (x7)
loop2:
	sta $0926,x
	inx
	iny
	cpy #$07
	bne loop2

	lda #$8e	; DLI (x1)
	sta $0926,x
	inx
	
	cpx #$60
	bne loop1

; 1 group of 5xHires + 1xADDR + 1xHires

	ldx #0
	lda #$0e	; Hires lines (x5)
loop3:
	sta $0986,x
	inx
	cpx #$05
	bne loop3

	lda #$4e	; Video Address line (x1)
	sta $098b
	lda #$00
	sta $098c
	lda #$80
	sta $098d
	
	lda #$0e	; Hires lines (x1)
	sta $098e
		
; 12 groups of 1xDLI + 7xHires lines
	ldx #0
loop5:
	
	lda #$8e	; DLI line (x1)
	sta $098f,x
	inx	
	
	ldy #0
	lda #$0e	; Hires lines (x7)
loop6:
	sta $098f,x
	inx	
	iny
	cpy #$07
	bne loop6
		
	cpx #$60
	bne loop5
	
; Footer
	lda #$41
	sta $09ef
	lda #$20
	sta $09f0
	lda #$09
	sta $09f1
	rts
.endproc
