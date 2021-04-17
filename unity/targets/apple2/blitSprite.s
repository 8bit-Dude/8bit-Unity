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

	.export _BlitSprite

; Zeropage addresses
mainAuxTog    = $42
hiresXZP      = $ec
hiresYZP      = $ed
hiresAddrZP   = $fc
inputAddrZP   = $fa
outputAddrZP  = $ee
scr2outRowsZP = $ce
inp2scrRowsZP = $eb
bytesPerRowZP = $e3

	.segment	"LC"	; MUST BE IN LANGUAGE CARD!!! (Area of memory not banked out during switches between AUX/MAIN memory)
	
; ---------------------------------------------------------------
; void __near__ BlitSprite (void)
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

.proc _BlitSprite: near

	; X loop: sprite rows
	ldx #0
.ifdef __DHR__
	stx mainAuxTog	; Init Main/Aux Toggle
.endif
loopRow:

		; Compute Screen Address using Hires Tables
		ldy hiresYZP			; Y-Offset to Hires Line
		lda _hiresLinesHI,y
		sta hiresAddrZP+1
		lda _hiresLinesLO,y
		clc
		adc hiresXZP			; X-Offset to Hires Byte
		sta hiresAddrZP

	.ifdef __DHR__
	branchAux:	
		; Select AUX memory
		sta $C000   	; Turn 80STORE off
		sta $C003   	; Read from AUX
		sta $C005   	; Write to AUX	
		jmp switchDone
	branchMain:
		; Select MAIN memory
		sta $C002   	; Read from MAIN
		sta $C004   	; Write to MAIN
		sta $C001   	; Turn 80STORE on
	switchDone:
	.endif
		
		; Copy bytes from DHR buffer to ouput	
	screen2output:
		lda outputAddrZP+1
		beq input2screen    ; If high-byte is zero, then skip
		ldy #0				; Y loop: Copy xxx bytes per row
	loopCopy1:
		lda (hiresAddrZP),y		; Copy 1 byte
		sta (outputAddrZP),y
		iny					
		cpy bytesPerRowZP
		bne loopCopy1			; Iterate Y loop
		
		; Copy bytes from input to DHR buffer
	input2screen:	
		cpx inp2scrRowsZP	; Check number of input rows (for cropped sprites)
		bcs doneCopy
		lda inputAddrZP+1
		beq doneCopy  		; If high-byte is zero, then skip	
		ldy #0				; Y loop: Copy xxx bytes per row
	loopCopy2:
		lda (inputAddrZP),y		; Copy 1 byte
		sta (hiresAddrZP),y
		iny					
		cpy bytesPerRowZP		; Iterate Y loop
		bne loopCopy2
		
	doneCopy:
	.ifdef __DHR__
		; Toggle AUX/MAIN
		lda mainAuxTog
		eor #1
		sta mainAuxTog
		bne branchMain
	.endif

	incAddress1:
		; Increment address of output block
		lda outputAddrZP			
		clc				
		adc bytesPerRowZP	; Move by xxx bytes
		sta outputAddrZP	
		bcc nocarry1		; Check if carry to high-byte
		inc outputAddrZP+1
	nocarry1:
		
	incAddress2:
		; Increment address of input block
		lda inputAddrZP			
		clc				
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
