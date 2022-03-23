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
	
	.export _sprRows, _sprPads, _sprDLIs, _sprBank, _sprDrawn
	.export _sprX, _sprY, _sprLine, _sprOff, _sprDst, _sprSrc, _sprColor	
	
	.import _posPM0, _colPM0	; See DLI.s

	.segment	"DATA"		
	
; Sprite parameters
_sprRows:   .byte 0
_sprPads:   .res  1
_sprDLIs:   .res  1
_sprX:      .res 16
_sprY:      .res 16
_sprBck:    .res 32
_sprDst:    .res 32
_sprSrc:    .res 32
_sprOff:    .res 16
_sprLine:   .res 16
_sprColor:  .res 16
_sprBank:	.byte 0,0,0,0
_sprFirst:	.byte 0,4,8,12
_sprLast:	.byte 4,8,12,16
_sprDrawn:  .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

	.segment	"BSS"			

; Working variables
_bank:  .res 1
_ind8:  .res 1
_ind16: .res 1
_ldli:  .res 1
_first: .res 1
_lead:  .res 1
_body:  .res 1
	
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
	; Decrement and save 8 bit index of slot
	dey
	sty _ind8

	; Check if slot enabled
	lda _sprDrawn,y	
	beq skipSlot
	
	; Set position/color registers (in case DLI gets disabled...)
	;tya
	;sec
	;sbc _first
	;tax
	;lda _sprX,y			; Sprite Position
	;sta $d000,x
	;lda _sprColor,y	 	; Sprite Color	
	;sta $02c0,x
	
	; Get amount of lead-zeroes
	lda _sprOff,y	 
	sta _lead	
	
	; Get amount of sprite-rows
	clc
	adc _sprRows	 
	sta _body
	
	; Get first (X-register) and last DLI Line
	lda _sprLine,y
	sta _ldli
	clc
	adc _sprDLIs
	tax
	
	; Set DLI lines
loopPosition:
	dex
	lda _sprX,y			; Sprite Position
	sta _posPM0,x
	lda _sprColor,y	 	; Sprite Color
	sta _colPM0,x
	cpx _ldli	
	bne loopPosition	
	
	; Arithmetic shift left to multiply index by 2 (16 bit addressing)
	tya
	asl				
	tay
	
	; Reset if sprite moved, then copy
	lda _sprBck,y
	cmp _sprDst,y
	beq skipReset
	jsr resetSprite
skipReset:
	jsr copySprite

skipSlot:
	; Next slot
	ldy _ind8
	cpy _first
	bne loopSlots
	
	rts
	
; ---------------------------------------------------------------
; Sprite reset routine (fills previous PMG memory with 0s)
; ---------------------------------------------------------------	
	
resetSprite:

	; Prepare addresses
	sty _ind16
	sta loopReset+2
	lda _sprDst,y
	sta _sprBck,y	
	iny
	lda _sprDst,y
	sta loopReset+3	 	
	
	; Reset PMG ram at old location
	lda #0
	ldy _sprPads
loopReset:
	dey
	sta $0000,y
	bne loopReset
	ldy _ind16

	rts

; ---------------------------------------------------------------
; Sprite copy routine (to PMG memory with lead/trail 0 padding)
; ---------------------------------------------------------------		
	
copySprite:

	; Prepare ZP addresses for copy
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
	bcc loopLead
skipLead:

	; Copy frame data
loopBody:
	lda $0000,y
	sta ($e8),y
	iny
	cpy _body
	bcc loopBody
	
	; Set trail-zeros	
	lda #0
loopTrail:	
	sta ($e8),y
	iny
	cpy _sprPads
	bcc loopTrail

	rts
	