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

	.export _StartDLI
	.export _countDLI
	.export _posPM0, _colPM0
	
	.import _charmapVBI
	.import _swapPalette
	.import _chrRows
	
		
; ROM addresses
vdslst = $0200
sdlstl = $0230	
nmien  = $d40e	
	
	.segment	"DATA"	

; DLI counter
_countDLI: .byte 0

; Sprite parameters
_posPM0: .res 25
_posPM1: .res 25
_posPM2: .res 25
_posPM3: .res 25
_colPM0: .res 25
_colPM1: .res 25
_colPM2: .res 25
_colPM3: .res 25

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
	; Backup A & X
	pha
	txa
	pha
	
	; Load line counter
	ldx _countDLI
	
	; Update position and color of players
	lda _posPM0,x  
	sta $d000
	lda _colPM0,x 
	sta $d012
	
	lda _posPM1,x  
	sta $d001
	lda _colPM1,x 
	sta $d013

	lda _posPM2,x  
	sta $d002
	lda _colPM2,x 
	sta $d014

	lda _posPM3,x  
	sta $d003
	lda _colPM3,x 
	sta $d015
	
	; Apply charmap/bitmap toggle?
	lda	_charmapVBI
	beq skipSwapPalette
	dex
	cpx _chrRows
	bne skipSwapPalette
	jsr _swapPalette
skipSwapPalette:
	
	; increment line counter
	inc _countDLI
		
	; Restore A & X
	pla
	tax
	pla
	rti
