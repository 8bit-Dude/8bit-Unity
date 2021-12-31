
;NES hardware-dependent functions by Shiru (shiru@mail.ru)
;with improvements by VEG
;Feel free to do anything you want with this code, consider it Public Domain

	.export _EncodePad
	
	.import _pad_poll
	
	.segment "DATA"

encoded: .res 1
	
	.segment "BANK0"

; ---------------------------------------------------------------
; unsigned char __fastcall__ EncodePad (unsigned char)
; ---------------------------------------------------------------	

_EncodePad:

	jsr _pad_poll
	
@encode:
	tay
	lda #255
	sta encoded
	
@encodeLeft:	
	
	tya
	and #2
	beq @encodeRight
	lda encoded
	eor #4
	sta encoded
	
@encodeRight:

	tya
	and #1
	beq @encodeUp
	lda encoded
	eor #8
	sta encoded

@encodeUp:	

	tya
	and #8
	beq @encodeDown
	lda encoded
	eor #1
	sta encoded

@encodeDown:	

	tya
	and #4
	beq @encodeA
	lda encoded
	eor #2
	sta encoded

@encodeA:	

	tya
	and #128
	beq @encodeB
	lda encoded
	eor #16
	sta encoded

@encodeB:	

	tya
	and #64
	beq @encodeSelect
	lda encoded
	eor #32
	sta encoded
	
@encodeSelect:	

	tya
	and #32
	beq @encodeStart
	lda encoded
	eor #64
	sta encoded

@encodeStart:	

	tya
	and #16
	beq @done
	lda encoded
	eor #128
	sta encoded
	
@done:

	lda encoded	
	rts
