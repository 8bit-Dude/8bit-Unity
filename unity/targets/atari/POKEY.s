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
	
	.export _pokeyVBI
	.export _musicVBI
	.export _sfxVBI

	.export _musicPaused
	
	.export _sampleTimer
	.export _sampleFreq
	.export _sampleCtrl
	
	.import _StartVBI
	.import _disable_rom
	.import _restore_rom	

RMTPlayer = $9400

	.segment	"DATA"	

; VBI toggles
_musicVBI:    .byte 0
_sfxVBI:      .byte 0

; Music parameters
_musicAddr:   .byte $00,$C0
_musicPaused: .byte 0

	.segment	"BSS"	

_sampleTimer: .res 4
_sampleFreq:  .res 4
_sampleCtrl:  .res 4

	.segment	"CODE"	
	
; ---------------------------------------------------------------
; void __near__ _PlayMusic (void)
; void __near__ _StopMusic (void)
; void __near__ _SetupSFX (void)
; ---------------------------------------------------------------	
		
_PlayMusic:
.ifdef __ATARIXL__
	; Setup track address
	ldx _musicAddr+0	; low byte of RMT module to X reg
	ldy _musicAddr+1    ; hi byte of RMT module to Y reg
	lda #0				; starting song line 0-255 to A reg
	jsr RMTPlayer		; Init: returns instrument speed (1..4 => from 1/screen to 4/screen)
	
	; Setup music playback
	lda #01
	sta _musicVBI
	jsr _StartVBI
.endif	
	rts

_StopMusic:
.ifdef __ATARIXL__
	; Disable VBI
	lda #00
	sta _musicVBI
	
	; Reset RMT player (all sounds off)
	jsr RMTPlayer+9
.endif	
	rts

_SetupSFX:
	; Setup SFX playback
	lda #01
	sta _sfxVBI
	jsr _StartVBI
	rts
	
; ---------------------------------------------------------------
; VBI routine
; ---------------------------------------------------------------

_pokeyVBI:
.ifdef __ATARIXL__
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
.endif
	
	;-------------------
	; Process SFX?
	lda _sfxVBI
	beq skipSFXVBI

	; Loop through channel 0-3
	ldy #0
	loopY:
		; Check sample timer
		ldx _sampleTimer,y
		cpx #0
		beq nextY

		; Continuous sound?
		cpx #255
		beq skipTimer
		
		; Decrement timer
		dex
		txa
		sta _sampleTimer,y
	skipTimer:		

		; Register offset (2*y)
		tya
		asl

		; Check if timer expired?
		cpx #0
		beq resetChannel
		
		; Set sample data
	setChannel:
		tax
		lda _sampleFreq,y
		sta $D200,x
		lda _sampleCtrl,y
		sta $D201,x
		jmp nextY
		
	resetChannel:
		tax
		lda #0
		sta $D200,x
		lda #0
		sta $D201,x
		
	nextY:
		iny
		cpy #4
		bne loopY

skipSFXVBI:
	rts
