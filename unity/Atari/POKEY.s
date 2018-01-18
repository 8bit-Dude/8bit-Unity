
	.include "atari.inc"
	
	.export _PlayMusic
	.export _StopMusic
	.export _PlaySFX
	.export _sampleCount
	.export _sampleFreq
	.export _sampleCtrl

RMTModule = $9000
RMTPlayer = $6A00

	.data
  
_sampleCount:
	.res 1
_sampleFreq:
	.res 1
_sampleCtrl:
	.res 1

	.code
		
.proc _PlayMusic: near
	lda #0				; starting song line 0-255 to A reg
	ldx #<RMTModule		; low byte of RMT module to X reg
	ldy #>RMTModule		; hi byte of RMT module to Y reg
	jsr RMTPlayer		; Init: returns instrument speed (1..4 => from 1/screen to 4/screen)

    lda #$06       	; immediate
	ldx #(>RMTVBI)	; install RMT VBI routine
	ldy #(<RMTVBI)
	;jsr $600	
    jsr SETVBV
	rts
.endproc

.proc _StopMusic: near
    lda #$06       	; immediate
	ldx #>$E45F		; reset VBI
	ldy #<$E45F		
    jsr SETVBV	

	jsr RMTPlayer+9	;all sounds off
	rts
.endproc

.proc _PlaySFX: near
    lda #$06       	; immediate
    ldx #(>SFXVBI)  ; install SFX VBI routine
    ldy #(<SFXVBI)	    
    jsr SETVBV
	rts
.endproc        


RMTVBI:
	; Play 1 note of the RMT
	jsr RMTPlayer+3	
	jmp SYSVBV
	

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
	jmp SYSVBV
