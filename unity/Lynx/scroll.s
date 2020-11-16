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

_tmpY: .res 1	
_tmpChr: .res 1
_tmpCol: .res 1
	
	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _Scroll (void)
;	Blit page from Charmap to Bitmap
;	Zero Page Data:
;		$b0: First Row
;		$b1: Last  Row
;		$b2: First Col
;		$b3: Last Col
;		$b4: Offset between charmap lines
;		$b5: 16 bit address of charmap line
;		$b7: 16 bit address of bitmap line
;		$b9: 16 bit address of charset data
;		$bb: 16 bit address of current charblock
; ---------------------------------------------------------------	

.proc _Scroll: near

; for (r=0; r<rows; r++) {
loopRows: 
	lda $b0			; Number of rows
	cmp $b1
	bpl doneRows
	inc $b0
	
		;-----------------------------
		; Reset Charblock address
		lda $b9
		sta $bb	
		lda $ba
		sta $bc
		
		; for (k=0; k<6; k++) {
			ldx #0
		loopLines:
			cpx #6
			beq doneLines	
			inx 
			
			; for (c=col1; c<col2; c++) {	
				ldy $b2
			loopCols:
				cpy $b3			; Number of cols
				bpl doneCols

				;---------------------
				; Get Char Value
				lda ($b5),y
				sty _tmpY
				
				;---------------------
				; Set Chr Offset
				asl A			; Multiply by 2	(2 bytes per col!)
				sta _tmpChr
				
				;----------------------
				; Set Col Offset
				tya
				asl A			; Multiply by 2	(2 bytes per col!)
				sta _tmpCol
				
				;----------------------
				; Copy 1st byte
				ldy _tmpChr
				lda ($bb),y		
				ldy _tmpCol				
				sta ($b7),y		

				;----------------------
				; Copy 2nd byte
				ldy _tmpChr
				iny 
				lda ($bb),y		
				ldy _tmpCol				
				iny 
				sta ($b7),y			

				;----------------------
				; Move to next col
				ldy _tmpY
				iny
				
			jmp loopCols	

		doneCols:

			; Update address of charblock (+256)
			inc $bc
		
			; Update address of bitmap line (+82)
			clc	
			lda $b7			; Update address of Hires line
			adc #82
			sta $b7
			bcc nocarryBM	; Check if carry to high byte
			inc $b8
		nocarryBM:		
		
		; Move to next line
		jmp loopLines
	
	doneLines:		
		
		; Update address of charmap line
		clc	
		lda $b5			
		adc $b4			; Add charmap line offset
		sta $b5	
		bcc nocarryCM	; Check if carry to high byte
		inc $b6
	nocarryCM:
	
	; Move to next row
	jmp loopRows

doneRows:
	rts

.endproc
