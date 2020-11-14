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
	
	.segment	"BSS"	
	
_tmp: .res 1
	
	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _Scroll (void)
;	Blit page from Charmap to Bitmap
;	Zero Page Data:
;		$b0: First Row
;		$b1: Last  Row
;		$b2: First Col
;		$b3: Last Col
;		$b4: Offset between charmap blocks
;		$b5: 16 bit address of charmap line
;		$b7: 16 bit address of bitmap line
;		$b9: 16 bit address of charset block
; ---------------------------------------------------------------	

.proc _Scroll: near

; for (a=0; a<rows; a++) {
loopRows: 
	lda $b0			; Number of rows
	cmp $b1
	beq doneRows
	inc $b0
	
	; add = CHARSETRAM
	lda #$00
	sta $b9
	lda #$7a
	sta $ba		

	; for (x=0; x<8; x++) {
		ldx #0
	loopLines:
		cpx #8
		beq doneLines	
		inx 
		
		; for (y=0; y<cols; y++) {	
			ldy $b2
		loopCols:
			cpy $b3			; Number of cols
			beq doneCols

				; chr = PEEK(src+i);
				lda ($b5),y		; Get char value
				
				; val = PEEK(add+chr)	
				sty _tmp
				tay 
				lda ($b9),y	
				ldy _tmp

				; POKE(line+i, val)
				sta ($b7),y					

			; Move to next col
			iny
			jmp loopCols

		doneCols:

			; Update charset block (+128)
		; add += 128;	
			clc	
			lda $b9			; Update address of char block
			adc #128
			sta $b9	
			bcc nocarryCS	; Check if carry to high byte
			inc $ba
		nocarryCS:
			
			; Update bitmap line (+40)
		; dst += 40;	
			clc	
			lda $b7			; Update address of bitmap line
			adc #40
			sta $b7
			bcc nocarryBM	; Check if carry to high byte
			inc $b8
		nocarryBM:
			
		; Move to next line
		jmp loopLines
	
	doneLines:

		; Update charmap line
	; src += charmapWidth
		clc	
		lda $b5			; Update address of charmap line
		adc $b4			; Add block offset
		sta $b5	
		bcc nocarryCM	; Check if carry to high byte
		inc $b6
	nocarryCM:
	
	; Move to next row
	jmp loopRows

doneRows:
	rts

.endproc
