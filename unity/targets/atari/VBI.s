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

	.export _StartVBI, _StopVBI 
	.export _charmapVBI, _spriteVBI
	.export _bmpAddr, _bmpPalette, _chrPalette, _SwapPalette
.ifdef __ATARIXL__	
	.export _bitmapVBI, _bmpToggle
.endif	

	;.export _parallaxRng, _parallaxSpd, _parallaxMax
			
	.import _BlitSprites
	.import _countDLI, _pokeyVBI
	;.import _parallaxDLI
	;.import _hScroll
.ifdef __CUSTOM_VBI__
	.import _CustomVBI
.endif

; ROM addresses
atract  = $004d
colSHW0 = $02c8
colSHW1 = $02c4
colSHW2 = $02c5
colSHW3 = $02c6
colSHW4 = $02c7
colHWR0 = $d01a
colHWR1 = $d016
colHWR2 = $d017
colHWR3 = $d018
colHWR4 = $d019
hscrol  = $d404	

	.segment	"DATA"	

; VBI parameters
.ifdef __ATARIXL__	
  _bitmapVBI:  .byte 0
  _bmpToggle:  .byte 0
.endif 
  _charmapVBI: .byte 0
  _spriteVBI:  .byte 0

; Bitmap/Charmap paramerers
_bmpAddr:	 .res  2
_bmpPalette: .byte $00, $24, $86, $d8, $00
_chrPalette: .byte $00, $24, $86, $d8, $00

; Parallax parameters
;_parallaxRng: .byte $10,$11,$13,$15,$19,0
;_parallaxSpd: .byte $83,$86,$87,$88,$89
;_parallaxMax: .byte $ff,$ff,$ff,$bf,$ff
;_parallaxRng: .byte 0,0,0,0,0,0
;_parallaxSpd: .byte 0,0,0,0,0
;_parallaxMax: .byte 0,0,0,0,0
;_parallaxVal: .byte 0,0,0,0,0
;_parallaxTmp: .res 2

	.segment	"CODE"

; ---------------------------------------------------------------
; void __near__ _StartVBI (void)
; ---------------------------------------------------------------	

.proc _StartVBI: near
	; Setup vertical blank interrupt
    lda #$07     ; deferred
	ldx #(>VBI)	 ; install VBI routine
	ldy #(<VBI)	
    jsr SETVBV
	rts
.endproc

; ---------------------------------------------------------------
; void __near__ _StopVBI (void)
; ---------------------------------------------------------------	

.proc _StopVBI: near
	; Setup vertical blank interrupt
    lda #$07       	; deferred
	ldx #(>XITVBV)  ; disable VBI routine
	ldy #(<XITVBV)	
    jsr SETVBV
	rts
.endproc

; ---------------------------------------------------------------
; VBI routine
; ---------------------------------------------------------------

VBI:
	; Reset atract (screen saver timer) and DLI counter
	lda #$00
	sta atract
	sta _countDLI
	
	; Compute parallax?
	;lda_parallaxDLI
	;beq skipParallax
	;jsr _ProcessParallax 
skipParallax:
		
	; Use double buffer bitmap?
.ifdef __ATARIXL__	
	lda _bitmapVBI
	beq skipBitmapVBI
	jsr _DoubleBuffer
.endif
skipBitmapVBI:	
	
	; Apply charmap/bitmap toggle?
	lda	_charmapVBI
	beq skipCharmapVBI
	jsr _SwapPalette
skipCharmapVBI:

	; Apply sprite flicker?
	lda _spriteVBI
	beq skipSpriteVBI
	jsr _BlitSprites
skipSpriteVBI:

	; Process POKEY sounds
	jsr _pokeyVBI
	
.ifdef __CUSTOM_VBI__
	; Process custom VBI
	jsr _CustomVBI
.endif

	; Exit VBI
	jmp XITVBV

; ---------------------------------------------------------------
; Bitmap flicker routine (switches between frame buffers)
; ---------------------------------------------------------------
.ifdef __ATARIXL__	
_DoubleBuffer:	
	; Toggle buffer 1/2
	lda _bmpToggle
	eor #$1
	sta _bmpToggle
	beq frame1

frame2:	
	; Switch bitmap buffer 1
	lda #$70 
	sta $0925
	lda #$80 
	sta $098d	
	rts

frame1:	
	; Switch bitmap buffer 2
	lda #$A0 
	sta $0925
	lda #$B0 
	sta $098d		
	rts
.endif
	
; ---------------------------------------------------------------
; Charmap/Bitmap palette swap routine
; ---------------------------------------------------------------

_SwapPalette:
	; Reg. A toggles Charmap/Bitmap
	beq palette2
	
palette1:
	; Switch to CHR palette for the top of screen
	lda _chrPalette+0
	sta  colSHW0
	lda _chrPalette+1
	sta  colSHW1
	lda _chrPalette+2
	sta  colSHW2
	lda _chrPalette+3
	sta  colSHW3
	lda _chrPalette+4
	sta  colSHW4
	rts
	
palette2:
	; Switch to BMP palette for the bottom of screen
	lda _bmpPalette+0
	sta  colHWR0
	lda _bmpPalette+1
	sta  colHWR1
	lda _bmpPalette+2
	sta  colHWR2
	lda _bmpPalette+3
	sta  colHWR3
	rts
	
; ---------------------------------------------------------------
; Parallax routine
; ---------------------------------------------------------------

_ProcessParallax:
	;ldx #0
	;ldy #0
loopIndx:	
	;lda _parallaxRng,x
	;beq initialScroll
	;sta _parallaxTmp
checkSign:
	;lda _parallaxSpd,x
	;cmp #$7f
	;bcc processPositive
processNegative:
	;and #$7f
	;sta _parallaxTmp+1
	;lda _parallaxVal,x
	;sbc _parallaxTmp+1
	;cmp _parallaxMax,x
	;bcc noResetNegative
	;lda _parallaxMax,x
noResetNegative:	
	;jmp processFraction
processPositive:
	;adc _parallaxVal,x
	;cmp _parallaxMax,x
	;bcc noResetPositive
	;lda #0
noResetPositive:
processFraction:
	;sta _parallaxVal,x
	;lsr A
	;lsr A
	;lsr A
	;lsr A
	loopLine:
	;	sta _hScroll,y
	;	iny
	;	cpy _parallaxTmp
	;	bne loopLine
		
	;inx
	;jmp loopIndx
initialScroll:
	;lda _hScroll
	;sta hscrol
	;rts
	