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

	.export _StartDLI, _StopDLI
	.export _countDLI, _spriteDLI
	.export _bmpRows, _chrRows
	
	;.export _parallaxDLI, _hScroll

	.export _posPM0, _colPM0
	.export _posPM1, _colPM1
	.export _posPM2, _colPM2
	.export _posPM3, _colPM3
	
	.import _SwapPalette
	
		
; ROM addresses
vdslst = $0200
sdlstl = $0230	
nmien  = $d40e
hscrol = $d404	
wsync  = $d40a
	
	.segment	"DATA"	

; DLI counters
_bmpRows:	   .byte 0
_chrRows:  	   .byte 0
_countDLI: 	   .byte 0
_spriteDLI:    .byte 0
;_parallaxDLI: .byte 0

; Sprite parameters
_posPM0: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
_posPM1: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
_posPM2: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
_posPM3: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
_colPM0: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
_colPM1: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
_colPM2: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
_colPM3: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

; Parallax parameters
;_hScroll: .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

	.segment	"CODE"

; ---------------------------------------------------------------
; void __near__ StartDLI (void)
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
; void __near__ StopDLI (void)
; ---------------------------------------------------------------	

.proc _StopDLI: near
	; Copy address of DLI routine
	sei
	lda	#$c0	
	sta nmien
    lda #$B3
	sta vdslst
    lda #$E7
	sta vdslst+1
	cli	
	rts
.endproc

; ---------------------------------------------------------------
; DLI routine
; ---------------------------------------------------------------

DLI:
	; Backup A & X
	pha
	txa
	pha
	
	; Load line counter
	ldx _countDLI	
	inc _countDLI
	
	; Update parallax?
	;lda _parallaxDLI
	;beq skipParallaxDLI	
	;sta wsync
	;lda _hScroll,x
	;sta hscrol
;skipParallaxDLI:	
	
	; Update sprites?	
	lda _spriteDLI
	beq skipCol3
	lda _posPM0,x  
	sta $d000
	beq skipCol0
	lda _colPM0,x 
	sta $d012
skipCol0:
	lda _posPM1,x  
	sta $d001
	beq skipCol1
	lda _colPM1,x 
	sta $d013
skipCol1:	
	lda _posPM2,x  
	sta $d002
	beq skipCol2
	lda _colPM2,x 
	sta $d014
skipCol2:	
	lda _posPM3,x  
	sta $d003
	beq skipCol3	
	lda _colPM3,x 
	sta $d015
skipCol3:	
	
	; Swap charmap/bitmap palette?
	cpx _chrRows
	bne skipPaletteDLI
	jsr _SwapPalette
skipPaletteDLI:
			
	; Restore A & X
	pla
	tax
	pla
	rti
