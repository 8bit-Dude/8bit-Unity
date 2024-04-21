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

	.include  "apple2.inc"
	
	.export _GetPaddle
	.export _GetPaddles
	.export _GetButton
	
	.export _paddleX, _paddleY

PREAD = $FB1E   ; Read paddle in X, return AD conv. value in Y

; Zeropage addresses	
_countX = $e3
	
	.segment	"BSS"	
	
_paddleX: .res 1	
_paddleY: .res 1	
	
	.segment	"CODE"		; DO NOT RELOCATE TO OTHER SEGMENTS!!! (app crashes)

; ---------------------------------------------------------------
; unsigned char __near__ _GetPaddle (unsigned char)
; ---------------------------------------------------------------		
	
.proc _GetPaddle: near
	; Read a particular paddle passed in A.
	bit     $C082           ; Switch in ROM
	
	; Read Paddle Potentiometer
	tax                     
	jsr     PREAD           ; Read paddle x
	tya
	ldx     #$00
	bit     $C080           ; Switch in LC bank 2 for R/O
	rts
.endproc	

; ---------------------------------------------------------------
; unsigned char __near__ _GetPaddles (unsigned char)
; ---------------------------------------------------------------	
.proc _GetPaddles: near
	asl						  ;select paddle 0 or 2
	tax
	ldy     #$00
	sty     _countX
	bit     PTRIG             ;reset counters
BothLoop:	
	lda 	PADDL0,x	
	bpl 	Paddle0Done
	lda 	PADDL1,x	
	bpl 	Paddle1Done
	inc		_countX
	iny
	bpl     BothLoop
	dec		_countX
	dey
Done:
	jmp     Next4
Paddle1Loop:
	nop
	nop
	nop
Paddle0Done:	
	lda 	PADDL1,x
	bpl     Done
	jmp     Next2
Next2:
	nop
	iny
	bpl     Paddle1Loop
	dey
	jmp     Next4
Paddle0Loop:
	lda 	PADDL0,x		
	bpl     Done
	nop
	nop
	nop
Paddle1Done:
	inc		_countX
	nop
	bpl     Paddle0Loop
	dec		_countX
	jmp     Next4
Next4:
	lda		_countX
	asl
	sta 	_paddleX
	tya
	asl
	sta 	_paddleY
	rts
.endproc

; ---------------------------------------------------------------
; unsigned char __near__ _GetButton (unsigned char)
; ---------------------------------------------------------------		
	
.proc _GetButton: near
	tax
	lda     BUTN0,x
	and		#$80	; Keep only bit 7
	asl				; Shift it to carry
	rol				; Roll carry to bit 0
	rts
.endproc	
