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

	.include "atari.inc"

	.export _BlitSprites
	
	.export _sprRows, _sprPads, _sprBank
	.export _sprBegDLI, _sprEndDLI, _sprLead, _sprDst, _sprSrc, _sprColor	
	
	.import _sprDrawn, _sprX, _sprY, _posPM0, _colPM0	; See DLI.s

	.segment	"DATA"		
	
; Sprite parameters
_sprRows:   .byte 0
_sprPads:   .res  1
_sprColor:  .res 16
_sprDst:    .res 32
_sprSrc:    .res 32
_sprLead:   .res 16
_sprBegDLI: .res 16
_sprEndDLI: .res 16
_sprBegOLD: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
_sprEndOLD: .byte 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
_sprBank:	.byte 0,0,0,0
_sprFirst:	.byte 0,4,8,12
_sprLast:	.byte 4,8,12,16

	.segment	"BSS"			

; Working variables
_bank:   .res 1
_slot:   .res 1
_first:  .res 1
_begdli: .res 1
_lead:   .res 1
_body:   .res 1
	
	.segment	"CODE"		
	
; ---------------------------------------------------------------
; Sprite blitting routine (switches between sprite banks)
; ---------------------------------------------------------------

_BlitSprites:

	; Toggle between slot banks 0, 1, 2, 3
bankLoop:	
	inc _bank			; Increase bank number
	ldx _bank
	cpx #4
	bcc bankCheck
	
	ldx #0
	stx _bank
	lda _sprBank
	bne bankDone
	rts
	
bankCheck:
	lda _sprBank,x		; Check if bank is enabled
	beq bankLoop
bankDone:

	; Assign frame range
	lda _sprFirst,x
	sta _first
	lda _sprLast,x
	tay
	
loopSlots:
	; Decrement slot
	dey

	; Check if slot enabled
	lda _sprDrawn,y	
	beq skipSlot
	sty _slot
		
	; Reset old DLI lines
	lda _sprBegOLD,y
	sta _begdli
	lda _sprEndOLD,y
	tax
	lda #0	
loopDLI1:
	dex
	sta _posPM0,x
	cpx _begdli	
	bne loopDLI1		
	
	; Set new DLI lines
	lda _sprBegDLI,y
	sta _sprBegOLD,y
	sta _begdli
	lda _sprEndDLI,y
	sta _sprEndOLD,y
	tax
loopDLI2:
	dex
	lda _sprX,y			; Sprite Position
	sta _posPM0,x
	lda _sprColor,y	 	; Sprite Color
	sta _colPM0,x
	cpx _begdli	
	bne loopDLI2	
	
	; Copy sprite data
	jsr copySprite

	; Next slot
	ldy _slot
skipSlot:
	cpy _first
	bne loopSlots
	
	rts

; ---------------------------------------------------------------
; Sprite copy routine (to PMG memory with lead/trail 0 padding)
; ---------------------------------------------------------------		
	
copySprite:

	; Get amount of lead-zeroes
	lda _sprLead,y	 
	sta _lead	
	
	; Get amount of sprite-rows
	clc
	adc _sprRows	 
	sta _body

	; Arithmetic shift left to multiply index by 2 (16 bit addressing)
	tya
	asl				
	tay

	; Prepare addresses for copy
	lda _sprDst,y	; Copy low-byte of address
	sta  $e8	 
	lda _sprSrc,y
	sta  loopBody+1	
	iny
	lda _sprDst,y	; Copy high-byte of address
	sta  $e9
	lda _sprSrc,y
	sta  loopBody+2	 

	; Set lead-zeros	
	lda #0
	ldy #0
	cpy _lead
	beq skipLead
loopLead:
	sta ($e8),y
	iny
	cpy _lead
	bne loopLead
skipLead:

	; Copy frame data
loopBody:
	lda $0000,y
	sta ($e8),y
	iny
	cpy _body
	bne loopBody
	
	; Set trail-zeros
	lda #0
loopTrail:	
	sta ($e8),y
	iny
	cpy _sprPads
	bcc loopTrail

	rts
	