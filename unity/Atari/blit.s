;
; Copyright (c) 2018 Anthony Beaucamp.
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

	.export _doubleHeight
	.export _sprRows
	.export _sprMask
	.export _sprX
	.export _sprY
	.export _sprColor
	.export _sprFrame
	
	.export _SetupFlickerDLI
	
	.segment	"DATA"	

_doubleHeight: .res 10
_doubleValue:  .res  1
_rowValue:     .res  1
_sprRows:  .res  1
_sprMask:  .res  5
_sprX:     .res 10
_sprY:     .res 10
_sprColor: .res 10
_sprFrame: .res 20
_sprOffset:.res  5
_sprToggle:.byte 1
_sprIndex: .byte 1

	.segment	"CODE"

; ---------------------------------------------------------------
; void __near__ _SetupFlickerDLI (void)
; ---------------------------------------------------------------	

.proc _SetupFlickerDLI: near
	; Copy address of update routine to Graphics Handler (see DLI.a65)
    lda #(<flickerSprites)
	sta $6ffc
    lda #(>flickerSprites)
	sta $6ffd
	rts
.endproc

; ---------------------------------------------------------------
; Flicker Routine, called by frame blending routine (see DLI.a65)
; 	Note: Uses 4 zero page registers in range $FC - $FF
; ---------------------------------------------------------------
	
flickerSprites:	

	;----------------------------
	; Toggle frame range: 0-4/5-9
	lda _sprToggle
	eor #3	; Toggle value flips between 1 and 2
	sta _sprToggle	
	
	;-----------------------------
	; Address of first Sprite Slot
	lda #$9c		; High-byte of first sprite slot ($9c00) 
	sta $fd			; Write to zero page for indirect addressing	
	sta $ff
	
	;-----------------
	; Sprite Slot Loop	
	ldx #0			; Init slot counter
slotLoopX:
	; Check mask for this slot
	lda _sprMask,x
	and _sprToggle
	beq nextSlot
	jsr drawSprite

	;-----------------------
	; Address of next sprite
nextSlot:
	cpx #3
	beq missileAddress
	inc $fd			; Increment high-byte to next sprite slot ($9c00 -> $9d00 -> $9e00 -> $9f00)
	inc $ff
	jmp doLoop
missileAddress:		
	lda #$9b		; Go back to $9b00 for 5th sprite
	sta $fd
	sta $ff
	
	;-----------------------
	; Increment to next Slot	
doLoop:	
	inx
	cpx #5
	beq done
	jmp slotLoopX	; Loop through X (5 sprites)
done:
	rts

; ---------------------------------------------------------------------------
; Actual sprite drawing routine (also takes care of cleaning previous sprite
; ---------------------------------------------------------------------------

drawSprite:
	;-----------------------
	; Handle frame range 1/2
	txa
	ldy _sprToggle
	cpy #1
	beq frameRange
	clc
	adc #5			; Frame range #2
frameRange:	
	tay
	
	;-----------------------
	; Copy Sprite Attributes
	cpx #4
	beq missile
player:
	lda _sprX,y		 ; X position
	sta $d000,x
	lda _sprColor,y	 ; Color
	sta $02c0,x			 ; Set Shadow Register
	sta $d012,x			 ; Also set hardware register (for larger sprites, routine may overrun the VBI...)
	jmp height
missile:
	lda _sprX,y		 ; X position
	sta $d007
	cpx #0
	adc #1
	sta $d006
	adc #2
	sta $d005
	adc #2
	sta $d004	
	lda _sprColor,y	 ; Color
	sta $02c7			 ; Set Shadow Register
	sta $d019			 ; Also set hardware register (for larger sprites, routine may overrun the VBI...)
height:
	lda _doubleHeight,y	 ; Use double height or not?
	sta _doubleValue
	sty _sprIndex
	
	;-------------------
	; Reset sprite slots
	lda _sprOffset,x 	 ; Offset of last sprite for this slot
	sta $fe				 ; Write to zero page for indirect addressing
	
	;-------------------
	; Eraze previous data
	ldy #0
	lda _doubleValue
	bne resetDouble

	; reset frame data (double height)
	lda #0				; Reset value
resetSingleY:	
	sta ($fe),y			; Write 0 to nth row of ith column
	iny
	cpy _sprRows
	bne resetSingleY	; Loop through Y (number of rows)	
	jmp copyFrame
	
	; reset frame data (double height)
resetDouble:	
	lda #0				; Reset value
resetDoubleY:	
	sta ($fe),y			; Write 0 to nth row of ith column
	inc $fe
	sta ($fe),y	
	iny
	cpy _sprRows
	bne resetDoubleY	; Loop through Y (number of rows)	
	
	;------------------
	; Copy Sprite Frame
copyFrame:	
	; Target Address for Data Copy
	ldy _sprIndex
	lda _sprY,y		 ; Low-byte = offset within slot
	sta _sprOffset,x ; Remember offset for later
	sta $fe				 ; Write to zero page for indirect addressing	

	; Source Address for Data Copy
	tya
	asl		; Use arithmetic shift left to multiply value from X by 2 (_sprFrame contains 16 bit addresses)
	tay		; Transfer multiplied offset to y and copy frame address to zero page for indirect addressing
	lda _sprFrame,y 
	sta $fc
	iny
	lda _sprFrame,y
	sta $fd	

	; Branch for single or double height
	ldy #0
	lda _doubleValue
	bne copyDoubleY

	; Copy frame data (single height)
copySingleY:	
	; Transfer 1 register
	lda ($fc),y	
	sta ($fe),y	
	iny
	cpy _sprRows
	bne copySingleY	; Loop through Y (number of rows)
	jmp return
	
	; Copy frame data (double height)
copyDoubleY:	
	; Transfer 1 register
	lda ($fc),y	
	sta ($fe),y	
	inc $fe
	sta ($fe),y	
	iny
	cpy _sprRows
	bne copyDoubleY	; Loop through Y (number of rows)
	
return:
	rts
