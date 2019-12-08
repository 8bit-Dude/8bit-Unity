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

	.import	pusha	
	
	.export _GetJoy	
	
PREAD = $FB1E   ; Read paddle in X, return AD conv. value in Y
THRESHOLD = 32  ; Deviation from center triggering movement

	.segment "LOWCODE"	
	
; ---------------------------------------------------------------
; unsigned char __near__ _GetJoy (unsigned char)
; ---------------------------------------------------------------		
	
.proc _GetJoy: near
	; READ: Read a particular joystick passed in A.
	bit     $C082           ; Switch in ROM
	and     #$01            ; Restrict joystick number

	; Joystick number -> paddle number
	asl                     

	; Read vertical paddle
	tax                     
	inx						; Set paddle number (1, 3)
	jsr     PREAD           ; Read paddle value
	lda     #$00            ; 0 0 0 0 0 0 0 0
	cpy     #127 - THRESHOLD
	ror                     ; !UP 0 0 0 0 0 0 0
	cpy     #127 + THRESHOLD
	ror                     ; DOWN !UP 0 0 0 0 0 0

	; Read horizontal paddle
	dex                     ; Set paddle number (0, 2)
	pha
	jsr     PREAD           ; Read paddle value
	pla
	cpy     #127 - THRESHOLD
	ror                     ; !LEFT DOWN !UP 0 0 0 0 0
	cpy     #127 + THRESHOLD
	ror                     ; RIGHT !LEFT DOWN !UP 0 0 0 0
	inx

	; Read primary button
	tay
	lda     BUTN0-1,x       ; Check button (1, 3)
	asl
	tya
	ror                     ; BTN1 RIGHT !LEFT DOWN !UP 0 0 0

	; Read secondary button
	tay
	inx
	txa
	and     #$03            ; IIgs has fourth button at TAPEIN
	tax
	lda     BUTN0-1,x       ; Check button (2, 0)
	asl
	tya
	ror                     ; BTN2 BTN1 RIGHT !LEFT DOWN !UP 0 0

	; Finalize
	eor     #%11101000      ; !BTN2 !BTN1 !RIGHT !LEFT !DOWN !UP 0 0
	ror
	ror						; 0 0 !BTN2 !BTN !DOWN !UP !RIGHT !LEFT	
	ldx     #$00
	bit     $C080           ; Switch in LC bank 2 for R/O
	rts
.endproc
