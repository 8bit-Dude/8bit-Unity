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

	.export _BlitSHR

hiresXZP = $ec
hiresYZP = $ed
hiresAddrZP = $fc
inputAddrZP = $fa
outputAddrZP = $ee
scr2outRowsZP = $ce
inp2scrRowsZP = $eb
bytesPerRowZP = $e3
toggleMainAuxZP = $42

	.segment	"LC"	; MUST BE IN LANGUAGE CARD!!! (see blitDHR.s)

; ---------------------------------------------------------------
; void __near__ BlitSHR (void)
;	Fast copy data between PROGRAM and Hires memory
;	Zero Page Data:
;		$e3: Number of bytes per row
;		$ce: Number of rows (output)
;		$eb: Number of rows (input)
;		$ec: Hires offset X
;		$ed: Hires offset Y
;		$ee: 16 bit output address (optional)
;		$fa: 16 bit input address (optional)
;
;		$fc: 16 bit address of Hires line (generated from offsets)
; ---------------------------------------------------------------	

.proc _BlitSHR: near

	; X loop: Frame rows
	ldx #0
loopRow:
	; Copy Screen Address from Hires Tables (using Line Offset Y and Byte Offset X)
	ldy hiresYZP			; Y-Offset to Hires Line
	lda _hiresLinesHI,y
	sta hiresAddrZP+1
	lda _hiresLinesLO,y
	adc hiresXZP			; X-Offset to Hires Byte
	sta hiresAddrZP
	
	; Copy bytes from SHR buffer to ouput	
screen2output:
	lda $ef
	beq input2screen  ; If high-byte is zero, then skip
	ldy #0				; Y loop: Copy xxx bytes per row
loopCopy1:
	lda (hiresAddrZP),y		; Copy 1 byte
	sta (outputAddrZP),y
	iny					
	cpy bytesPerRowZP
	bne loopCopy1			; Iterate Y loop
	
	; Copy bytes from input to SHR buffer
	cpx inp2scrRowsZP		; Check number of input rows (for cropped sprites)
	bcs incAddress1
input2screen:	
	clc
	lda $fb
	beq incAddress1   ; If high-byte is zero, then skip	
	ldy #0				; Y loop: Copy xxx bytes per row
loopCopy2:
	lda (inputAddrZP),y		; Copy 1 byte
	sta (hiresAddrZP),y
	iny					
	cpy bytesPerRowZP		; Iterate Y loop
	bne loopCopy2
		
incAddress1:
	clc				; Increment address of output block
	lda outputAddrZP			
	adc bytesPerRowZP	; Move by xxx bytes
	sta outputAddrZP	
	bcc nocarry1		; Check if carry to high-byte
	inc outputAddrZP+1
nocarry1:
	
incAddress2:
	clc				; Increment address of input block
	lda inputAddrZP			
	adc bytesPerRowZP	; Move by xxx bytes
	sta inputAddrZP	
	bcc nocarry2		; Check if carry to high byte
	inc inputAddrZP+1
nocarry2:

nextRow:
	; Move to next row
	inc hiresYZP		; Increment Hires Line offset
	inx				
	cpx scr2outRowsZP
	bcc loopRow		; Iterate X loop (rows)
	
	rts
.endproc
