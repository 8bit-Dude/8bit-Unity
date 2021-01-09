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

	.export _BlitDHR

_mainAuxTog: .res 1

	.segment	"LOWCODE"
	
; ---------------------------------------------------------------
; void __near__ BlitDHR (void)
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

.proc _BlitDHR: near

	; X loop: Frame rows
	ldx #0
	stx _mainAuxTog		; Init Main/Aux Toggle
loopRow:
	; Copy from Hires Tables (with Line Offset Y and Byte Offset X) to $fc/$fd
	ldy $ed				; Y offset within table
	lda _hiresLinesHI,y
	sta $fd
	lda _hiresLinesLO,y
	adc $ec				; Add X Offset
	sta $fc

	; Main/Aux Toggle
branchAux:	
	sta $c055		; Switch to AUX memory
	jmp switchDone
branchMain:
	sta $c054		; Switch to MAIN memory
switchDone:
	
	; Copy bytes from DHR buffer to ouput	
screen2output:
	lda $ef
	beq input2screen  ; If high-byte is zero, then skip
	ldy #0			; Y loop: Copy ? bytes (see $e3)
loopCopy1:
	lda ($fc),y		; Copy 1 byte
	sta ($ee),y
	iny				; Iterate Y loop
	cpy $e3
	bne loopCopy1
	
incAddress1:
	clc				; Increment address of output block
	lda $ee			
	adc $e3			; Move ? bytes (see $e3)
	sta $ee	
	bcc nocarry1	; Check if carry to high-byte
	inc $ef
nocarry1:
	
	; Copy bytes from input to DHR buffer
	cpx $eb			; Check number of input rows (for cropped sprites)
	bcs toggleBlocks
input2screen:	
	clc
	lda $fb
	beq toggleBlocks  ; If high-byte is zero, then skip	
	ldy #0			; Y loop: Copy ? bytes (see $e3)
loopCopy2:
	lda ($fa),y		; Copy 1 byte
	sta ($fc),y
	iny				; Iterate Y loop
	cpy $e3
	bne loopCopy2
	
incAddress2:
	clc				; Increment address of input block
	lda $fa			
	adc $e3			; Move ? bytes (see $e3)
	sta $fa	
	bcc nocarry2	; Check if carry to high byte
	inc $fb
nocarry2:
	
	; Toggle AUX/MAIN
toggleBlocks:
	clc
	lda _mainAuxTog
	eor #1
	sta _mainAuxTog
	bne branchMain

nextRow:
	; Move to next row
	inc $ed			; Increment Y-Line offset in Hires Table
	inx				; Iterate X loop
	cpx $ce
	bcc loopRow	
	rts
.endproc
