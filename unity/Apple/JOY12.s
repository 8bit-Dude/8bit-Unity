
	.include  "apple2.inc"

	.import	pusha	
	
	.export _GetJoy12	

PREAD = $FB1E   ; Read paddle in X, return AD conv. value in Y
THRESHOLD = 20  ; Deviation from center triggering movement

	.code
	
; ---------------------------------------------------------------
; unsigned char __near__ _GetJoy12 (unsigned char)
; ---------------------------------------------------------------		
	
.proc _GetJoy12: near
	; READ: Read a particular joystick passed in A.
	bit     $C082           ; Switch in ROM
	and     #$01            ; Restrict joystick number

	; Read horizontal paddle
	asl                     ; Joystick number -> paddle number
	tax                     ; Set paddle number (0, 2)
	jsr     PREAD           ; Read paddle value
	lda     #$00            ; 0 0 0 0 0 0 0 0
	cpy     #127 - THRESHOLD
	ror                     ; !LEFT 0 0 0 0 0 0 0
	cpy     #127 + THRESHOLD
	ror                     ; RIGHT !LEFT 0 0 0 0 0 0

	; Read vertical paddle
	pha
	inx                     ; Set paddle number (1, 3)
	jsr     PREAD           ; Read paddle value
	pla
	cpy     #127 - THRESHOLD
	ror                     ; !UP RIGHT !LEFT 0 0 0 0 0
	cpy     #127 + THRESHOLD
	ror                     ; DOWN !UP RIGHT !LEFT 0 0 0 0

	; Read primary button
	tay
	lda     BUTN0-1,x       ; Check button (1, 3)
	asl
	tya
	ror                     ; BTN DOWN !UP RIGHT !LEFT 0 0 0

	; Read secondary button
	tay
	inx
	txa
	and     #$03            ; IIgs has fourth button at TAPEIN
	tax
	lda     BUTN0-1,x       ; Check button (2, 0)
	asl
	tya
	ror                     ; BTN2 BTN DOWN !UP RIGHT !LEFT 0 0

	; Finalize
	eor     #%11101000      ; !BTN2 !BTN !DOWN !UP !RIGHT !LEFT 0 0
	ldx     #$00
	bit     $C080           ; Switch in LC bank 2 for R/O
	rts
.endproc
	