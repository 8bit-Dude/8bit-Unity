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

	.export _SpriteCopy

	.segment	"DATA"
		
_dhrLinesHI: 
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
 	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
 	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
 	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	
_dhrLinesLO: 
	.byte $00,$00,$00,$00,$00,$00,$00,$00
 	.byte $80,$80,$80,$80,$80,$80,$80,$80
	.byte $00,$00,$00,$00,$00,$00,$00,$00
 	.byte $80,$80,$80,$80,$80,$80,$80,$80
	.byte $00,$00,$00,$00,$00,$00,$00,$00
 	.byte $80,$80,$80,$80,$80,$80,$80,$80
	.byte $00,$00,$00,$00,$00,$00,$00,$00
 	.byte $80,$80,$80,$80,$80,$80,$80,$80
 	.byte $28,$28,$28,$28,$28,$28,$28,$28
 	.byte $A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8
 	.byte $28,$28,$28,$28,$28,$28,$28,$28
 	.byte $A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8
 	.byte $28,$28,$28,$28,$28,$28,$28,$28
 	.byte $A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8
 	.byte $28,$28,$28,$28,$28,$28,$28,$28
 	.byte $A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8
 	.byte $50,$50,$50,$50,$50,$50,$50,$50
 	.byte $D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0
 	.byte $50,$50,$50,$50,$50,$50,$50,$50
 	.byte $D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0
 	.byte $50,$50,$50,$50,$50,$50,$50,$50
 	.byte $D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0
 	.byte $50,$50,$50,$50,$50,$50,$50,$50
 	.byte $D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0

_mainAuxTog: .res 1

	.segment	"CODE"
	
; ---------------------------------------------------------------
; void __near__ SpriteCopy (void)
;	Copy sprite to DHR memory (optionally backup the screen)
;	Zero Page Data:
;		$eb: Number of rows
;		$ec: DHR offset X
;		$ed: DHR offset Y
;		$fa: 16 bit address of Backup (optional)
;		$fa: 16 bit address of Sprite
; ---------------------------------------------------------------	

.proc _SpriteCopy: near

	; Init Main/Aux Toggle
	lda #0
	sta _mainAuxTog

	; X loop: Number of lines
	ldx $eb
loopX:
	; Copy from DHR Tables (with Line Offset Y and Byte Offset X) to $fc/$fd
	ldy $ed				; Y offset within table
	lda _dhrLinesHI,y
	sta $fd
	lda _dhrLinesLO,y
	adc $ec				; Add X Offset
	sta $fc

	; Main/Aux Toggle
branchAux:	
	sta $c055		; Switch to AUX memory
	jmp switchDone
branchMain:
	sta $c054		; Switch to MAIN memory
switchDone:
	
	; Copy 2 bytes from screen to backup	
	lda $ef
	beq source2screen	; If high-byte is zero, then skip
screen2backup:
	ldy #0			; Y loop: Copy 2 bytes
loopCopy1:
	lda ($fc),y		; Copy 1 byte
	sta ($ee),y
	iny				; Iterate Y loop
	cpy #2
	bne loopCopy1
	
	; Copy 2 bytes from source to screen
source2screen:	
	ldy #0			; Y loop: Copy 2 bytes
loopCopy2:
	lda ($fa),y		; Copy 1 byte
	sta ($fc),y
	iny				; Iterate Y loop
	cpy #2
	bne loopCopy2

	; Increment address of source block
	clc
	lda $fa			
	adc #2			; Move 2 bytes
	sta $fa	
	bcc nocarry1	; Check if carry to high byte
	inc $fb
nocarry1:
	
	; Increment address of backup block
	lda $ef
	beq nocarry2	; If high-byte is zero, then skip
	clc
	lda $ee			
	adc #2			; Move 2 bytes
	sta $ee	
	bcc nocarry2	; Check if carry to high-byte
	inc $ef
nocarry2:

	; Process Main Block?
	lda _mainAuxTog
	eor #1
	sta _mainAuxTog
	bne branchMain

	; Move to next row
	inc $ed			; Increment Y-Line offset in DHR Table
	dex				; Iterate X loop
	bne loopX	
.endproc
