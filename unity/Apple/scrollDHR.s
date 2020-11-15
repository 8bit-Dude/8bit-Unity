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

	.import _hiresLinesHI, _hiresLinesLO
	
	.export _ScrollDHR
	
	.segment	"BSS"	
	
_tmp: .res 1
_mainAuxTog: .res 1
	
	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _ScrollSHR (void)
;	Blit page from Charmap to Bitmap
;	Zero Page Data:
;		$e3: First Row
;		$eb: Last  Row
;		$ec: First Col
;		$ed: Last Col
;		$ee: Offset between charmap lines
;		$ef: 16 bit address of charmap line (auto-updated)
;
;		$d7: Current Line (auto-generated)
;		$ce: 16 bit address of Hires line (auto-generated)
;		$fb: 16 bit address of charset block L (auto-generated)
;		$fd: 16 bit address of charset block L (auto-generated)
; ---------------------------------------------------------------	

_ScrollDHR:

; Set start line
	lda $e3
	asl A
	asl A
	asl A
	sta $d7

; for (a=row1; a<row2; a++) {
loopRows: 
	lda $e3			; Number of rows
	cmp $eb
	bpl doneRows
	inc $e3
	
	; add = CHARSETRAM (L)
	lda #$00
	sta $fb
	lda #$88
	sta $fc

	; add = CHARSETRAM (R)
	lda #$00
	sta $fd
	lda #$90
	sta $fe

	; for (x=0; x<8; x++) {
		ldx #0
	loopLines:
		cpx #8
		beq doneLines	
		inx
		
			;---------------------
			; Set Hires address of current line
			ldy $d7				; Current line
			lda _hiresLinesHI,y
			sta $cf
			lda _hiresLinesLO,y
			sta $ce
			inc $d7
			
			;---------------------
			; Main/Aux Toggle
		branchAux:	
			sta $c055		; Switch to AUX memory
			jmp switchDone
		branchMain:
			sta $c054		; Switch to MAIN memory
		switchDone:			
		
			;---------------------
			; Copy 1 line
			jsr copyLine

			;---------------------
			; Update address of charset block L (+128)
			clc	
			lda $fb			
			adc #128
			sta $fb	
			bcc nocarryCL	; Check if carry to high byte
			inc $fc
		nocarryCL:

			;---------------------
			; Update address of charset block R (+128)
			clc	
			lda $fd			
			adc #128
			sta $fd	
			bcc nocarryCR	; Check if carry to high byte
			inc $fe
		nocarryCR:
			
			;---------------------
			; Toggle AUX/MAIN
			clc
			lda _mainAuxTog
			eor #1
			sta _mainAuxTog
			bne branchMain			
			
		; Move to next line
		jmp loopLines
	
	doneLines:

		; Update address of charmap line
		clc	
		lda $ef			
		adc $ee			; Add charmap line offset
		sta $ef	
		bcc nocarryCM	; Check if carry to high byte
		inc $f0
	nocarryCM:
	
	; Move to next row
	jmp loopRows

doneRows:
	rts


;----------------------------------
; Copy 1 line of data (AUX or MAIN)
copyLine:
; for (y=col1; y<col2; y++) {	
	ldy $ec
loopCols:
	cpy $ed			; Number of cols
	bpl doneCols
	
		;---------------------
		; Handle Left Char
		lda ($ef),y		; Get char value
		sty _tmp		
		tay 
		lda ($fb),y		; Get L pixels for that char
		ldy _tmp
		sta ($ce),y		; Save in Hires mem

		; Move to next col
		iny

		;---------------------
		; Handle Right Char
		lda ($ef),y		; Get char value
		
		sty _tmp
		tay 
		lda ($fd),y		; Get R pixels for that char
		ldy _tmp
		sta ($ce),y		; Save in Hires mem

		; Move to next col
		iny
		
	jmp loopCols

doneCols:
	rts
	
