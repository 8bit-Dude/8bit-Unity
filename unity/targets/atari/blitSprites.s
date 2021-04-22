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
	
	.export _sprToggle, _sprRows, _sprPads, _sprDLIs, _sprBank, _sprDrawn
	.export _sprX, _sprY, _sprLine, _sprOff, _sprDst, _sprSrc, _sprColor	
	
	.import _posPM0, _colPM0	; See DLI.s

	.segment	"DATA"		
	
; Sprite parameters
_sprToggle: .byte 2
_sprRows:   .byte 0
_sprPads:   .res  1
_sprDLIs:   .res  1
_sprBank:	.res  3
_sprX:      .res 12
_sprY:      .res 12
_sprBck:    .res 24
_sprDst:    .res 24
_sprSrc:    .res 24
_sprOff:    .res 12
_sprLine:   .res 12
_sprColor:  .res 12
_sprDrawn:  .byte 0,0,0,0,0,0,0,0,0,0,0,0	

	.segment	"BSS"			

; Working variables
_bank:  .res 1
_ind8:  .res 1
_ind16: .res 1
_ldli:  .res 1
_first: .res 1
_last:  .res 1
_lead:  .res 1
_body:  .res 1
	
	.segment	"CODE"		
	
; ---------------------------------------------------------------
; Sprite blitting routine (switches between sprite banks)
; ---------------------------------------------------------------

_BlitSprites:

	; Process only every other frame
	lda _sprToggle
	eor #$1
	sta _sprToggle
	bne processSprites
	rts
processSprites:

	; Toggle between slot banks 0, 1, 2
bankLoop:	
	inc _bank
	lda _bank
	cmp #3
	bcc bankCheck
	lda #0
	sta _bank
	lda _sprBank	
	bne bankDone
	rts
bankCheck:
	tax
	lda _sprBank, x
	beq bankLoop
bankDone:
	
	; Increase index by 4 for each banks
	lda #0		; Frame range #00-#03
	ldx #0
	cpx _bank
	beq rangeDone
	clc
	adc #4		; Frame range #04-#07 
	inx
	cpx _bank
	beq rangeDone
	clc
	adc #4		; Frame range #08-#11 
rangeDone:	

	; Save first index of range
	sta _first  
	tay			

	; Save last index of range
	clc
	adc #4
	sta _last

loopSlots:
	; Save 8 bit index
	sty _ind8

	; Check if slot enabled
	lda _sprDrawn,y	
	beq skipSlot
	
	; Set position/color registers (in case DLI gets disabled...)
	tya
	sec
	sbc _first
	tax
	lda _sprX,y			; Sprite Position
	sta $d000,x
	lda _sprColor,y	 	; Sprite Color	
	sta $02c0,x
	
	; Get amount of lead-zeroes
	lda _sprOff,y	 
	sta _lead	
	
	; Get amount of body-rows
	clc
	adc _sprRows	 
	sta _body
	
	; Get first (X-register) and last DLI Line
	lda _sprLine,y
	tax
	clc
	adc _sprDLIs
	sta _ldli
	
	; DLI position
loopPosition:
	lda _sprX,y			; Sprite Position
	sta _posPM0,x
	lda _sprColor,y	 	; Sprite Color
	sta _colPM0,x
	inx
	cpx _ldli	
	bcc loopPosition	
	
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
	iny
	cpy _last
	bcc loopSlots
	
	rts
	
; ---------------------------------------------------------------
; Sprite reset routine (fills previous PMG memory with 0s)
; ---------------------------------------------------------------	
	
resetSprite:

	; Prepare ZP address for resetting
	sty _ind16
	sta  $e8
	lda _sprDst,y
	sta _sprBck,y	
	iny
	lda _sprDst,y
	sta  $e9	 	
	
	; Reset PMG ram at old location
	lda #0
	ldy #0
loopReset:
	sta ($e8),y
	iny
	cpy _sprPads
	bcc loopReset
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
	sta  $ea	
	iny
	lda _sprDst,y	; Copy high-byte of address
	sta  $e9	 
	lda _sprSrc,y
	sta  $eb

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
	lda ($ea),y
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
	