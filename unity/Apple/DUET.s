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
; Adapted from: Electric Duet Player Routine circa 1980 (Copyright (C) 1979-2015 Paul Lutus)
;

	.export _PlaySpeaker
	.export _UpdateSpeaker

	.segment "CODE"	
		
; ---------------------------------------------------------------
; void __near__ _PlaySpeaker (unsigned int address)
; ---------------------------------------------------------------

_PlaySpeaker:
		
		; Setup address (and back it up)
		STA $1E
		STA $AAFE
		STX $1F
		STX $AAFF
		RTS

; ---------------------------------------------------------------
; void __near__ _UpdateSpeaker (void)
; ---------------------------------------------------------------
	
_UpdateSpeaker:
		
		; Player Setup
play:	LDA #$01        ; 2 *!*
		STA $09         ; 3
		STA $1D         ; 3
		PHA             ; 3
		PHA             ; 3
		PHA             ; 3
		BNE lbl1        ; 4 *!*
lbl0:   INY             ; 2
		LDA ($1E),Y     ; 5 *!*
		STA $09         ; 3
		INY             ; 2
		LDA ($1E),Y     ; 5 *!*
		STA $1D         ; 3
lbl22:	LDA $1E         ; 3 *!*
		CLC             ; 2
		ADC #$03        ; 2 *!*
		STA $1E         ; 3
		BCC lbl1       ; 4 *!*
		INC $1F         ; 5
lbl1:   LDY #$00        ; 2 *!*
		LDA ($1E),Y     ; 5 *!*
		CMP #$01        ; 2
		BEQ lbl0       ; 4 *!*
		BCS lbl2       ; 4 *!*
		PLA             ; 4
		PLA             ; 4
		PLA             ; 4
lbl4:   LDX #$49        ; 2 *!*
		INY             ; 2
		LDA ($1E),Y     ; 5 *!*
		BNE lbl3       ; 4 *!*
		LDX #$c9        ; 2 *!*
lbl3:   RTS
lbl2:   STA $08         ; 3
		JSR lbl4       ; 6
		STX lbl5       ; 4
		STA $06         ; 3
		LDX $09         ; 3 *!*
lbl6:	LSR A           ; 2
		DEX             ; 2
		BNE lbl6       ; 4 *!*
		STA lbl7+1       ; 4		
		JSR lbl4       ; 6
		STX lbl8       ; 4
		STA $07         ; 3
		LDX $1D         ; 3 *!*
lbl9:	LSR A           ; 2
		DEX             ; 2
		BNE lbl9       ; 4 *!*
		STA lbl10+1       ; 4		
		PLA             ; 4
		TAY             ; 2
		PLA             ; 4
		TAX             ; 2
		PLA             ; 4
		BNE lbl11       ; 4 *!*
lbl24:	BIT $C030       ; 4
lbl11:	CMP #$00        ; 2
		BMI lbl12       ; 4 *!*
		NOP             ; 2
		BPL lbl13       ; 4 *!*
lbl12:	BIT $C030       ; 4
lbl13:	STA $4E         ; 3
		BIT $C000       ; 4	
		BMI lbl3       ; 4 *!*
		DEY             ; 2
		BNE lbl7       ; 4 *!*
		BEQ lbl14       ; 4 *!*
lbl7:	CPY #$00        ; 2
		BEQ lbl5       ; 4 *!*
		BNE lbl15       ; 4 *!*
lbl14:	LDY $06         ; 3 *!*
lbl5:	EOR #$40        ; 2 *!*
lbl15:	BIT $4E         ; 3
		BVC lbl16       ; 4 *!*
		BVS lbl17       ; 4 *!*
lbl17:	BPL lbl18       ; 4 *!*
		NOP             ; 2
		BMI lbl19       ; 4 *!*
lbl16:	NOP             ; 2
		BMI lbl18       ; 4 *!*
		NOP             ; 2
		BPL lbl19       ; 4 *!*
lbl18:	CMP $C030       ; 4
lbl19:	DEC $4F         ; 5
		BNE lbl20       ; 4 *!*
		DEC $08         ; 5
		BNE lbl20       ; 4 *!*
		BVC lbl21       ; 4 *!*
		BIT $C030       ; 4
lbl21:	PHA             ; 3
		TXA             ; 2
		PHA             ; 3
		TYA             ; 2
		PHA             ; 3
		JMP lbl22       ; 3
lbl20:	DEX             ; 2
		BNE lbl10       ; 4 *!*
		BEQ lbl23       ; 4 *!*
lbl10:	CPX #$00        ; 2
		BEQ lbl8       ; 4 *!*
		BNE lbl25       ; 4 *!*
lbl23:	LDX $07         ; 3 *!*
lbl8:	EOR #$80        ; 2 *!*
lbl25:	BVS lbl24       ; 4 *!*
		NOP             ; 2
		BVC lbl11       ; 4 *!*
		RTS
