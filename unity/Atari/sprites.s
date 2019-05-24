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

	.export _flickerRows
	.export _flickerMask
	.export _flickerX
	.export _flickerY
	.export _flickerColor
	.export _flickerFrame
	
	.export _SetupFlickerDLI
	
	.segment	"DATA"	

_flickerRows:  .res 1
_flickerMask:  .res 5
_flickerX:     .res 10
_flickerY:     .res 10
_flickerColor: .res 10
_flickerFrame: .res 20
_flickerOffset:.res 5
_flickerSel:   .byte 1

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
	lda _flickerSel
	eor #3	; Toggle value flips between 1 and 2
	sta _flickerSel	
	
	;-----------------
	; Sprite Slot Loop
	lda #$9c		; High-byte of first sprite slot ($9c00) 
	sta $ff			; Write to zero page for indirect addressing	
	ldx #0			; Init slot counter
slotLoopX:
	; Check mask for this slot
	lda _flickerMask,x
	and _flickerSel
	beq nextSlot

	;-------------------
	; Reset sprite slots
	lda _flickerOffset,x ; Offset of last sprite for this slot
	sta $fe				 ; Write to zero page for indirect addressing
	
	; Eraze previous data
	ldy _flickerRows	 ; Sprite length
	lda #$00			 ; Reset value
resetLoopY:	
	sta ($fe),y		; Write 0 to nth row of ith column
	dey
	bne resetLoopY	; Loop through Y (number of rows)
	
	;-----------------------
	; Handle frame range 1/2
	txa
	ldy _flickerSel
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
	lda _flickerX,y		 ; X position
	sta $d000,x
	lda _flickerColor,y	 ; Color
	sta $02c0,x			 ; Set Shadow Register
	sta $d012,x			 ; Also set hardware register (for larger sprites, routine may overrun the VBI...)
	jmp copyFrame
missile:
	lda _flickerX,y		 ; X position
	sta $d007
	cpx #0
	adc #1
	sta $d006
	adc #2
	sta $d005
	adc #2
	sta $d004	
	lda _flickerColor,y	 ; Color
	sta $02c7			 ; Set Shadow Register
	sta $d019			 ; Also set hardware register (for larger sprites, routine may overrun the VBI...)
	
	;------------------
	; Copy Sprite Frame
copyFrame:	
	; Target Address for Data Copy
	lda _flickerY,y		 ; Low-byte = offset within slot
	sta _flickerOffset,x ; Remember offset for later
	sta $fe				 ; Write to zero page for indirect addressing	

	; Source Address for Data Copy
	tya
	asl		; Use arithmetic shift left to multiply value from X by 2 (_flickerFrame contains 16 bit addresses)
	tay		; Transfer multiplied offset to y and copy frame address to zero page for indirect addressing
	lda _flickerFrame,y 
	sta $fc
	iny
	lda _flickerFrame,y
	sta $fd	
	
	; Copy frame data
	ldy #0
copyLoopY:	
	; Transfer 1 register
	lda ($fc),y	
	sta ($fe),y	
	iny
	cpy _flickerRows
	bne copyLoopY	; Loop through Y (number of rows)

nextSlot:
	lda $ff
	cmp #$9f
	beq missileAddress
	inc $ff			; Increment high-byte to next sprite slot ($9c00 -> $9d00 -> $9e00 -> $9f00)
	jmp doLoop
missileAddress:
	lda #$9b
	sta $ff
doLoop:	
	inx
	cpx #5
	beq done
	jmp slotLoopX	; Loop through X (5 sprites)
done:
	rts
