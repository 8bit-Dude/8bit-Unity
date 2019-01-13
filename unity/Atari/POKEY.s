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

	.include "atari.inc"
	
	.export _PlayMusic
	.export _StopMusic
	.export _SetupSFX
	.export _sampleCount
	.export _sampleFreq
	.export _sampleCtrl

RMTPlayer = $6A00

	.segment	"DATA"	

_tmp: .res 1
_sampleCount: .res 1
_sampleFreq:  .res 1
_sampleCtrl:  .res 1

	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _PlayMusic (unsigned int address)
; void __near__ _StopMusic (void)
; void __near__ _SetupSFX (void)
; ---------------------------------------------------------------	
		
.proc _PlayMusic: near
	; Shuffling registers
	sta _tmp
	txa
	tay					; hi byte of RMT module to Y reg
	ldx _tmp			; low byte of RMT module to X reg
	lda #0				; starting song line 0-255 to A reg
	jsr RMTPlayer		; Init: returns instrument speed (1..4 => from 1/screen to 4/screen)

    lda #$07       	; deferred
	ldx #(>RMTVBI)	; install RMT VBI routine
	ldy #(<RMTVBI)
	;jsr $600	
    jsr SETVBV
	rts
.endproc

.proc _StopMusic: near
    lda #$07       	; deferred
	ldx #>XITVBV	; reset VBI
	ldy #<XITVBV		
    jsr SETVBV	

	jsr RMTPlayer+9	;all sounds off
	rts
.endproc

.proc _SetupSFX: near
    lda #$07       	; deferred
    ldx #(>SFXVBI)  ; install SFX VBI routine
    ldy #(<SFXVBI)	    
    jsr SETVBV
	rts
.endproc        


RMTVBI:
	; Play 1 note of the RMT
	jsr RMTPlayer+3	
	jmp XITVBV
	

SFXVBI:
	; Get samples counter
	ldx _sampleCount
	cpx #0
	beq done
	
	; Set sample data
	lda _sampleFreq
	sta $D206
	lda _sampleCtrl
	sta $D207

	; Increment counter
	dex
	stx _sampleCount
	cpx #0
	bne done

reset:	
	lda #0
	sta $D206
	lda #0
	sta $D207
	
done:
	; do the normal interrupt service routine
	jmp XITVBV
