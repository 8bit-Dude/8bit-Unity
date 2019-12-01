;
; Copyright (c) 2019 Anthony Beaucamp.
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
; Adapted from the abcmusic library by Karri Kaksonen, 02.02.2019
;
; The macro package is by Bjoern Spruck. Taken from Chipper.
;

;----------------------------------------------------------------------------
;
; For direct sound control, use these function in your C code
; extern void __fastcall__ abcoctave(unsigned char chan, unsigned char val);
; extern void __fastcall__ abcpitch(unsigned char chan, unsigned char val);
; extern void __fastcall__ abctaps(unsigned char chan, unsigned int val);
; extern void __fastcall__ abcintegrate(unsigned char chan, unsigned char val);
; extern void __fastcall__ abcvolume(unsigned char chan, unsigned char val);
; 

	.export _abctimers
	.export _abcoctave
	.export _abcpitch
	.export _abctaps
	.export _abcintegrate
	.export _abcvolume

	.interruptor _abcmusic_irq

	.import popa

;----------------------------------------------------------------------------
;
; Variables
;
	.segment "DATA"
endoftune:			; Dummy "empty" tune used for stopping play
	.byte 0
_abcactive:			; Read only active flags for 4 channels
	.byte 0,0,0,0
_abctimers:			; SFX Timer data	(MOD by 8bit-Dude)
	.byte 0,0,0,0
abc_music_ptr:		; Score pointer
	.byte 0,0,0,0
abc_repeat_offs:	; Something to do with repeat of sections
	.byte 0,0,0,0
abc_repeat_cnt:		; Something to do with repeat of sections again
	.byte 0,0,0,0
abc_tempo:			; Store tempo of every score 'T'
	.byte 6,6,6,6
abc_channel_volume:		; Store volume if every score 'V'
	.byte 127,127,127,127
abc_instrument_incr:		; Store attack increment step 'R'
	.byte 4,4,4,4
abc_instrument_maxlen:		; Store how long to hold the note 'H'
	.byte 4,4,4,4
abc_instrument_decr:		; Store decay decrement step 'K'
	.byte 4,4,4,4
abc_note_length:		; Total note length from start of attack until end of decay
	.byte 6,6,6,6
sound_channel_current_volume:	; Current volume of channel
	.byte 127,127,127,127
note_playing:			; Current length of note playing
	.byte 4,4,4,4
cur_note:			; Temporary variable
	.byte 0
cur_octave:			; Temporary variable
	.byte 0

	.segment "RODATA"
; This table is used to cover the delays needed for 1 octave
_delays:
	.byte 255 ; C
	.byte 240 ;^C
	.byte 227 ; D
	.byte 214 ;^D
	.byte 202 ; E
	.byte 191 ;^E
	.byte 191 ; F
	.byte 180 ;^F
	.byte 170 ; G
	.byte 161 ;^G
	.byte 152 ; A
	.byte 143 ;^A
	.byte 135 ; B
	.byte 127 ;^B

	.segment "BSS"
instrumenttmp:
	.res 2
abc_number:
	.res 2
abc_tunestr:
	.res 2
abc_oct:
	.res 4
abc_tmp2:
	.res 1

	.segment "CODE" 
;----------------------------------------------------------------------------
; Macros
;

if_count	.set	0
nest_count	.set	0

.macro	_IFEQ
	if_count	.set	if_count +1
	nest_count	.set	nest_count +1
	bne		.ident (.sprintf ("else%04d", if_count))
	.ident (.sprintf ("push%04d", nest_count)) .set if_count
.endmacro

.macro  _ELSE
	bra	.ident (.sprintf ("endif%04d", .ident (.sprintf ("push%04d", nest_count))))
	.ident  (.sprintf ("else%04d", .ident (.sprintf ("push%04d", nest_count)))) := *
.endmacro

.macro  _ENDIF
	.if .not .defined( .ident (.sprintf ("else%04d", .ident (.sprintf ("push%04d", nest_count)))))
		.ident  (.sprintf ("else%04d", .ident (.sprintf ("push%04d", nest_count)))) := *
	.endif
	.ident  (.sprintf ("endif%04d", .ident (.sprintf ("push%04d", nest_count)))) := *
	nest_count	.set	nest_count -1
.endmacro

;----------------------------------------------------------------------------
;
; Integrate flag
;
integrate:
	phx
	pha
	txa
	clc
	rol
	clc
	rol
	clc
	rol
	tax
	pla
	cmp #0
	_IFEQ
		lda $fd25,x	; Square wave
		and #$df
	_ELSE
		lda $fd25,x	; Triangle wave
		ora #$20
	_ENDIF
	ora #$18
	sta $fd25,x
	plx
	rts

;----------------------------------------------------------------------------
;
; Choose square wave or integrate
; extern void __fastcall__ abcintegrate(unsigned char chan, unsigned char val);
;
_abcintegrate:
	pha
	jsr popa   ; pop chan
	tax
	pla
	jmp integrate

;----------------------------------------------------------------------------
;
; Process polynomic sound generator taps
;
taps:
	phy
	tay
	phx
	txa ; modify X to point to sound channel
	clc
	rol
	clc
	rol
	clc
	rol
	tax
	tya
	sta $fd21,x
	plx
	ply
	rts

taps40:
	phy
	tay
	phx
	txa ; modify X to point to sound channel
	clc
	rol
	clc
	rol
	clc
	rol
	tax
	; Bit $40 is put in a different register
	tya
	and #$40
	_IFEQ
		lda $fd25,x
		and #$7f
	_ELSE
		lda $fd25,x
		ora #$80
	_ENDIF
	ora #$18
	sta $fd25,x
	lda #0
	sta $fd23,x
	plx
	ply
	rts

;----------------------------------------------------------------------------
;
; Choose polynomic feedback taps
; extern void __fastcall__ abctaps(unsigned char chan, unsigned int val);
;
_abctaps:
	pha	; Push original abc_number low byte
	; The two topmost bits are shifted one place right
	bpl @L1
		ora #$40
		bra @L2
@L1:
		and #$bf
@L2:
	dex
	bne @L3
		ora #$80
		bra @L4
@L3:
		and #$7f
@L4:
	pha		; Get channel number to X
	jsr popa
	tax
	pla
	jsr taps	; Set most taps bits
	pla		; Get bit 40 that goes to different register
	lda #0
	jsr taps40
	rts

;----------------------------------------------------------------------------
;
; Set the BACKUP register on the hw to start running out the pitch
; X - channel to use
; A - desired backup value
;
backup:
	phx
	pha
	txa
	clc
	rol
	clc
	rol
	clc
	rol
	tax
	pla
	sta $fd24,x
	plx
	rts

;----------------------------------------------------------------------------
;
; Choose pitch of the current sound
; extern void __fastcall__ abcpitch(unsigned char chan, unsigned char val);
;
_abcpitch:
	pha
	jsr popa   ; pop chan
	tax
	pla
	jmp backup

;----------------------------------------------------------------------------
;
; Set the current octave
; X - channel to use
; A - desired octave
;
setoctave:
	phy
	tay
	phx
	txa
	clc
	rol
	clc
	rol
	clc
	rol
	tax
	lda $fd25,x
	and #$f8
	ora #$18
	iny
@L1:
	dey
	beq @L2
	ina
	bra @L1
@L2:
	sta $fd25,x
	plx
	ply
	rts

;----------------------------------------------------------------------------
;
; Choose octave 0..6
; extern void __fastcall__ abcoctave(unsigned char chan, unsigned char val);
;
_abcoctave:
	pha
	jsr popa   ; pop chan
	tax
	pla
	jmp setoctave

; Run time envelope processing for a note
; X is the channel
; return A, 0 note ended
envelope:
  lda note_playing,x
  bne @L0
  rts					; Note has ended
@L0:
  cmp #255				; Duration is forever
  beq @L3
    dea					; Decrement the duration of the note
    sta note_playing,x
    bne @L1
      sta sound_channel_current_volume,x ; Turn off volume at end of note
      bra @L5
@L1:
  lda abc_note_length,x
  sec
  sbc note_playing,x
  cmp abc_instrument_maxlen,x
  bcc @L3
    lda sound_channel_current_volume,x	; Past the H time. Start decaying
    sec
    sbc abc_instrument_decr,x
    bcs @L2
      ; silence
      lda #0
@L2:
    sta sound_channel_current_volume,x	; Set current decayed volume
    bra @L5
@L3:
  lda sound_channel_current_volume,x
  cmp abc_channel_volume,x
  bcs @L5
    clc					; We are in the attack phase
    adc abc_instrument_incr,x
    bcc @L4
      lda sound_channel_current_volume,x ; desired volume reached
@L4:
    sta sound_channel_current_volume,x
@L5:
  lda sound_channel_current_volume,x
volume:
	phx
	pha
	txa
	clc
	rol
	clc
	rol
	clc
	rol
	tax
	pla
	sta $fd20,x
	plx
	lda #1
	rts

;----------------------------------------------------------------------------
;
; Choose pitch of the current sound
; extern void __fastcall__ abcvolume(unsigned char chan, unsigned char val);
;
_abcvolume:
	pha
	jsr popa   ; pop chan
	tax
	pla
	jmp volume

;----------------------------------------------------------------------------
;
; Last, but no least - tie update mechanism to interrupts

VBL_INTERRUPT = $04
INTSET      = $FD81

_abcmusic_irq:	lda	INTSET
	and	#VBL_INTERRUPT
	beq	@exit
	jsr	abctimers	;MOD by 8bit-Dude
@exit:
	clc
	rts
	
; Run-time processing of SFX timers
abctimers:
	ldx #0
@L1:	
	lda _abctimers,x	
	beq @L2
		dea
		sta _abctimers,x
		bne @L2
			jmp volume		
@L2:	
	inx
	txa
	cmp #4
	bne @L1
	rts
	