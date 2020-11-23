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

	.export _musicPaused
	.export _sampleCount
	.export _sampleFreq
	.export _sampleCtrl
	
	.import _disable_rom
	.import _restore_rom	

	.import _musicAddr

RMTPlayer = $9400

	.segment	"DATA"	

_musicPaused: .byte 0
_musicVBI:    .byte 0
_sfxVBI:      .byte 0

	.segment	"BSS"	

_sampleCount: .res 1
_sampleFreq:  .res 1
_sampleCtrl:  .res 1

	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _PlayMusic (void)
; void __near__ _StopMusic (void)
; void __near__ _SetupSFX (void)
; ---------------------------------------------------------------	
		
_PlayMusic:
	; Setup track address
	ldx _musicAddr+0	; low byte of RMT module to X reg
	ldy _musicAddr+1    ; hi byte of RMT module to Y reg
	lda #0				; starting song line 0-255 to A reg
	jsr RMTPlayer		; Init: returns instrument speed (1..4 => from 1/screen to 4/screen)
	
	; Setup music playback
	lda #01
	sta _musicVBI
	jsr _StartVBI
	rts

_StopMusic:
	; Disable VBI
	lda #00
	sta _musicVBI
	
	; Reset RMT player (all sounds off)
	jsr RMTPlayer+9
	rts

_SetupSFX:
	; Setup SFX playback
	lda #01
	sta _sfxVBI
	jsr _StartVBI
	rts
	
_StartVBI:
	; Setup vertical blank interrupt
    lda #$07       	; deferred
	ldx #(>VBI)	; install RMT VBI routine
	ldy #(<VBI)	
    jsr SETVBV
	rts

; ---------------------------------------------------------------
; VBI routine
; ---------------------------------------------------------------

VBI:
	;-------------------
	; Process music?
	lda _musicVBI
	beq skipMusicVBI
	
	; Check if track is paused
	lda _musicPaused
	beq playFrame
	
	; Stop sound for now
	jsr RMTPlayer+9
	jmp skipMusicVBI

playFrame:
	; Play 1 note of the RMT
	jsr _disable_rom
	jsr RMTPlayer+3
	jsr _restore_rom
	
skipMusicVBI:
	
	;-------------------
	; Process SFX?
	lda _sfxVBI
	beq skipSFXVBI

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

	; Reset channel
	lda #0
	sta $D206
	lda #0
	sta $D207
skipSFXVBI:

done:
	; Exit VBI
	jmp XITVBV
