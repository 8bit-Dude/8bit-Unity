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

	.export _StartDLI
	
	.export _bitmapDLI
	.export _bmpPalette
	.export _frameBlending
	
	.export _charmapDLI
	.export _chrPalette

	.export _spriteDLI
	.export _doubleHeight
	.export _sprRows
	.export _sprMask
	.export _sprX
	.export _sprY
	.export _sprColor
	.export _sprFrame
	
; ROM addresses
atract = $004d
vdslst = $0200
sdlstl = $0230	
nmien  = $d40e	

; Char palette
colorSHADOW0 = $02c8
colorSHADOW1 = $02c4
colorSHADOW2 = $02c5
colorSHADOW3 = $02c6
colorSHADOW4 = $02c7

colorHARDWR0 = $d01a
colorHARDWR1 = $d016
colorHARDWR2 = $d017
colorHARDWR3 = $d018
colorHARDWR4 = $d019
	
	.segment	"DATA"	

; DLI toggles
_bitmapDLI:  .byte 0
_charmapDLI: .byte 0
_spriteDLI:  .byte 0

; Bitmap parameters
_bmpPalette: 	.byte $00, $24, $86, $d8
_frameBlending: .byte 0

; Bitmap paramerers
_chrPalette: .res  5
_chrToggle:  .byte 0

; Sprite parameters
_doubleHeight:  .byte 0,0,0,0,0,0,0,0,0,0
_sprRows:  		.byte 0
_sprMask:  		.byte 0,0,0,0,0
_sprX:     		.byte 0,0,0,0,0,0,0,0,0,0
_sprY:     		.byte 0,0,0,0,0,0,0,0,0,0
_sprColor: 		.byte 0,0,0,0,0,0,0,0,0,0
_sprFrame: 		.res 20
_sprOffset:		.res  5
_sprToggle:		.byte 1
_sprIndex: 		.byte 1
_doubleValue:   .res  1

; Registers backup
regA: .byte 1
regX: .byte 1
regY: .byte 1	

	.segment	"CODE"

; ---------------------------------------------------------------
; void __near__ _StartDLI (void)
; ---------------------------------------------------------------	

.proc _StartDLI: near
	; Copy address of DLI routine
	sei
	lda	#$c0	
	sta nmien
    lda #(<DLI)
	sta vdslst
    lda #(>DLI)
	sta vdslst+1
	cli	
	rts
.endproc

; ---------------------------------------------------------------
; DLI routine
; ---------------------------------------------------------------

DLI:
	; Backup registers
	sta regA
	stx regX
	sty regY
	
	; Process Charset swap?
	lda	_charmapDLI
	beq skipCharmapDLI
	jsr swapPalette
	lda _chrToggle
	beq skipCharmapDLI  ; Exit early from first DLI
	lda regA
	ldx regX
	ldy regY	
	rti
skipCharmapDLI:
	
	; Process bitmap flicker?
	lda _bitmapDLI
	beq skipBitmapDLI
	jsr flickerFrames
skipBitmapDLI:

	; Process sprite flicker?
	lda _spriteDLI
	beq skipSpriteDLI
	jsr flickerSprites
skipSpriteDLI:	

	; Reset atract (screen saver timer)
	lda #$00
	sta atract

	; Restore Registers
	lda regA
	ldx regX
	ldy regY
	rti

; ---------------------------------------------------------------
; Charset swap routine
; ---------------------------------------------------------------
	
swapPalette:
	lda _chrToggle
	eor #$1
	sta _chrToggle
	bne charset2
	
charset1:
	; Switch to CHR palette for the top of screen
	lda _chrPalette+0
	sta  colorSHADOW0
	lda _chrPalette+1
	sta  colorSHADOW1
	lda _chrPalette+2
	sta  colorSHADOW2
	lda _chrPalette+3
	sta  colorSHADOW3
	lda _chrPalette+4
	sta  colorSHADOW4
	rts
	
charset2:
	; Wait till we reach the next line of text
	;sta $d40A ; WSYNC
	;sta $d40A ; WSYNC
	;sta $d40A ; WSYNC
	;sta $d40A ; WSYNC
	;sta $d40A ; WSYNC
	;sta $d40A ; WSYNC
	;sta $d40A ; WSYNC
	
	; Switch to BMP palette for the bottom of screen
	lda _bmpPalette+0
	sta  colorHARDWR0
	lda _bmpPalette+1
	sta  colorHARDWR1
	lda _bmpPalette+2
	sta  colorHARDWR2
	lda _bmpPalette+3
	sta  colorHARDWR3
	rts
	
; ---------------------------------------------------------------
; Bitmap flicker routine (switches between frame buffers)
; ---------------------------------------------------------------

flickerFrames:	
	; Toggle buffer 1/2
	lda _frameBlending
	eor #$1
	sta _frameBlending
	bne showframe2

showframe1:	
	; Switch bitmap buffer 1
	lda #$A0 
	sta $0905
	lda #$B0 
	sta $096d	
	rts

showframe2:	
	; Switch bitmap buffer 2
	lda #$70 
	sta $0905
	lda #$80 
	sta $096d		
	rts

; ---------------------------------------------------------------
; Sprite flicker Routine (called by DLI routine if enabled)
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
