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
; Adapted from: Electric Duet Music Player for the Mockingboard (COPYRIGHT 2014 CYBERNESTO)
;				Mockingboard Demo (COPYRIGHT Steve Nickolas, Michael Pohoreski)
;

	.export _DetectMocking
	.export _InitMocking
	.export _PlayMocking
	.export _StopMocking
	.export _SFXMocking

	.segment "LOWCODE"	

; Mockingboard Base Address
ADDR6522        = $C400      ; *** Default Mockingboard to slot 4 ***

; Left Speaker
MOCK1_OR_B      = ADDR6522+0 ; OR Register B
MOCK1_OR_A      = ADDR6522+1 ; OR Register A

; LOW ORDER COUNTER
LOWORDER_COUNT  = ADDR6522+4 ; Used for detection

; 6522 Commands          DIR BC1 BC0
_6522_RESET      = $0   ; 0   0   0
_6522_PORT_B     = $4   ; 1   0   0
_6522_WRITE      = $6   ; 1   1   0
_6522_LATCH      = $7   ; 1   1   1

; Registers
MOCK_REG_FIRST  = $0
MOCK_REG_LAST   = $D
MOCK_REG_NUM    = $E

; Electric Duet
CHN       = $1D
SONG      = $1E
LEFTCHN   = $00
RIGHTCHN  = $02
ENAREG    = $07
VOL_A     = $08
VOL_B     = $09
TONE      = $06
DURATION  = $08

OCTAVE:   .byte 1
TEMPO:    .byte 8
TEMP:     .byte 0


; ---------------------------------------------------------------
; void __near__ _DetectMocking () 	
; ---------------------------------------------------------------

; Look for mocking board in slot 4
.proc _DetectMocking: near	

	ldx	#02	; 2 tries?
	
mb4_check_cycle_loop:
	lda	LOWORDER_COUNT	; timer 6522 (low order counter)
						; count down
	sta	TEMP			; 3 cycles
	lda	LOWORDER_COUNT	; + 5 cycles = 8 cycles
						; between the two accesses to the timer
	sec
	sbc	TEMP			; subtract to see if we had 8 cycles
	cmp	#$f8			; -8
	bne	mb4_not_in_this_slot
	dex					; decrement, try one more time
	bne	mb4_check_cycle_loop	; loop detection
	
done_mb4_detected:
	lda #01				; Mockingboard found (X=1)
	rts					; return

mb4_not_in_this_slot:
	lda	#00
	rts
	
.endproc

; ---------------------------------------------------------------
; void __near__ _InitMocking ()
; ---------------------------------------------------------------

.proc _InitMocking: near	

	LDA #$FF
	STA $C403
	STA $C483
	LDA #$07
	STA $C402
	STA $C482
	RTS
		  
.endproc

; ---------------------------------------------------------------
; void __near__ _StopMocking ()
; ---------------------------------------------------------------

.proc _StopMocking: near	

	LDA #$00
	STA $C400
	STA $C480
	LDA #$04
	STA $C400
	STA $C480
	RTS
  
.endproc
	
; ---------------------------------------------------------------
; void __near__ _PlayMocking (unsigned int address)
; ---------------------------------------------------------------

.proc _PlayMocking: near	

	; Check high byte: if not 0 set address, otherwise play music
	CPX #$00
	BEQ play

	; Setup address (and back it up)
	STA $1E
	STA $AAFE
	STX $1F
	STX $AAFF
	JSR RESET
	RTS
		
; Electric Duet Player Setup
play:     
	JSR RESET
	JSR ENACHN
	JMP LOOP

SETVOL:
NEXT:     
	LDA SONG
	CLC
	ADC #$03
	STA SONG
	BCC LOOP
	INC SONG+1
LOOP:     
	LDY #$00
	LDA (SONG),Y
	CMP #$01
	BEQ SETVOL
	BPL SETNOTE    ;SET DURATION
END:      
;-- Check Key Interrupt --
	LDA $C000
	CMP #$80
	BCS exit
;--- Loop track ---
	LDA $AAFE
	STA $1E
	LDA $AAFF
	STA $1F
	JMP play
;--- Exit playback ---
exit:
	JSR RESET
	RTS		  		  
;-------------------
		  
SETNOTE:  
	STA DURATION
    LDA #LEFTCHN
SEND:     
	STA CHN
	STA $C401
	JSR SETREG1
	INY
	LDA (SONG),Y
	;BEQ SKIP       ;IF 0 KEEP LTTSA (Modified: Reset LTTSA!)
	JSR CONVFREQ
SKIP:     
	LDA TONE
	STA $C401
	JSR WRDATA1
	INC CHN
	LDA CHN
	STA $C401
	JSR SETREG1
	LDA TONE+1
	STA $C401
	JSR WRDATA1
	LDA #RIGHTCHN
	STA CHN
	CPY #$02
	BNE SEND
	LDX DURATION
W1:      
	LDY TEMPO
W2:       
	DEC TEMP
	BNE W2
	DEY
	BNE W2
	DEX
	BNE W1
	BIT $C000
	BMI END
	JMP NEXT

CONVFREQ: 
	LDX OCTAVE
	INX
	PHA
	LDA #$00
	STA TONE+1
	PLA
DECOCT:   
	DEX
	BMI LOBYTE
	ASL
	ROL TONE+1
	JMP DECOCT
LOBYTE:   
	STA TONE
	RTS
		  
RESET:    
	LDA #$00
	STA $C400
	STA $C480
	LDA #$04
	STA $C400
	STA $C480
	RTS

SETREG1:  
	LDA #$07
	STA $C400
	LDA #$04
	STA $C400
	RTS

WRDATA1:  
	LDA #$06
	STA $C400
	LDA #$04
	STA $C400
	RTS

ENACHN:   
	LDA #ENAREG
	STA $C401
	JSR SETREG1
	LDA #%00111100
	STA $C401
	JSR WRDATA1
	LDA #VOL_A
	STA $C401
	JSR SETREG1
	LDA #$0F
	STA $C401
	JSR WRDATA1
	LDA #VOL_B
	STA $C401
	JSR SETREG1
	LDA #$0F
	STA $C401
	JSR WRDATA1
	RTS  
.endproc
			
; ---------------------------------------------------------------
; void __near__ _SFXMocking (unsigned int address)
; ---------------------------------------------------------------
; Plays one note by setting all Mockingboard registers
;   X = Low  Address of registers
;   Y = High Address

.proc _SFXMocking: near
            STA _mock1_load_reg+1        ; *** SELF-MODIFIES!
            STX _mock1_load_reg+2			
_mock1all_regs:
            ldy #0
            ldx #MOCK_REG_FIRST
_mock1_load_reg:
            lda mockN_off_data,X    ; *** SELF-MODIFIED!
            jsr MOCK1_PokeXA
            inx
            cpx #MOCK_REG_NUM       ; BUG: $B !
            bcc _mock1_load_reg
            rts
.endproc

; === DATA ===
mockN_off_data:
            BRK                     ; Reg 0,1 Tone A
            BRK
            BRK                     ; Reg 2,3 Tone B
            BRK
            BRK                     ; Reg 4,5 Tone C
            BRK
            BRK                     ; Reg 6 Noise
            BRK                     ; Reg 7 Enable
            BRK                     ; Reg 8 Amp A
            BRK                     ; Reg 9 Amp B
            BRK                     ; Reg A Amp C
            BRK                     ; Reg B Env Low
            BRK                     ; Reg C Env Hi
            BRK                     ; Reg D Env Type
			
; ====================
; Plays one note
; IN:
;   X = Register: $0 .. $D
;   A = Value
; OUT:
;   A = trash
;   X = unchanged
;   Y = trash
; ====================
MOCK1_PokeXA:
            TAY
            ; NOTE: Intentional fall-into!

; ====================
; Poke (set) byte to Mockingboard
; IN:
;   X = Register: $0 .. $D
;   Y = Value
; OUT:
;   A = trash
;   X = unchanged
;   Y = unchanged
; ====================
MOCK1_PokeXY:
            stx MOCK1_OR_A
            jsr MOCK1_SetReg
            sty MOCK1_OR_A
			jmp MOCK1_SetData
MOCK1_SetReg:                       ;         |
            lda #_6522_LATCH        ;-->+     |
            .byte $2C               ;   |     |
MOCK1_SetData:                      ;   |     |
            lda #_6522_WRITE        ;   |     |
            sta MOCK1_OR_B          ;<--+  <--+
            lda #_6522_PORT_B
            sta MOCK1_OR_B
            rts
			
