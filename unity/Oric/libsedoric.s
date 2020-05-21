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

;
; Generated with ap65 v 1.00	(souce: OSDK)
;

	.setcpu     "65816"
	.smart      on
	.autoimport on
	.case       on
	
	.importzp   sp, sreg, regsave, regbank
	.importzp   tmp1, tmp2, tmp3, tmp4
	.importzp   ptr1, ptr2, ptr3, ptr4
	
	.export _exvec
	.export _dosrom
	.export _dosflag
	.export _doserr
	.export _sed_fname
	.export _sed_begin
	.export _sed_end
	.export _sed_size
	.export _sed_err
	.export _sed_command
	.export _sed_savefile
	.export _sed_loadfile
	.export _cls
	
_exvec =  $02f5
_dosrom =  $04f2
_dosflag =  $04fc
_doserr =  $04fd

.code
_sed_fname: .byte 0,0
_sed_begin: .byte 0,0
_sed_end: .byte 0,0
_sed_size: .byte 0,0
_sed_err: .byte 0,0
savebuf_zp: .res 256


_sed_savefile:
	tya
	pha
	jsr sed_savezp
	lda _sed_fname
	sta $e9
	lda _sed_fname+1
	sta $ea
	jsr _dosrom
	lda #1
	sta $0b
	lda #$00
	sta $c018
	clc
	lda #$00
	jsr $d454
	lda _sed_begin
	sta $c052
	lda _sed_begin+1
	sta $c053
	lda _sed_end
	sta $c054
	lda _sed_end+1
	sta $c055
	lda _sed_begin
	sta $c056
	lda _sed_begin+1
	sta $c057
	lda #$00
	sta $c04d
	lda #$40
	sta $c04e
	lda #$40
	sta $c051
	jsr $de0b
	jsr _dosrom
	jsr sed_restorezp
	lda _doserr
	sta _sed_err
	lda _doserr+1
	sta _sed_err+1
	pla
	tay
	rts

_sed_loadfile:
	tya
	pha
	jsr sed_savezp
	lda _sed_fname
	sta $e9
	lda _sed_fname+1
	sta $ea
	jsr _dosrom
	lda #1
	sta $0b
	lda #$00
	sta $c018
	clc
	lda #$00
	jsr $d454
	lda _sed_begin
	sta $c052
	lda _sed_begin+1
	sta $c053
	lda #$00
	sta $c04d
	lda #$40
	sta $c04e
	jsr $e0e5
	lda $c052
	sta _sed_begin
	lda $c053
	sta _sed_begin+1
	clc
	lda $c04f
	sta _sed_size
	adc _sed_begin
	sta _sed_end
	lda $c050
	sta _sed_size+1
	adc _sed_begin+1
	sta _sed_end+1
	jsr _dosrom
	jsr sed_restorezp
	lda _doserr
	sta _sed_err
	lda _doserr+1
	sta _sed_err+1
	pla
	tay

sed_savezp:
	ldx #00
	L_139_loop:
	lda $00,x
	sta savebuf_zp,x
	dex
	bne L_139_loop
	rts

sed_restorezp:
	ldx #00

L_150_loop:
	lda savebuf_zp,x
	sta $00,x
	dex
	bne L_150_loop
	rts

_cls:
	jmp $ccce
	
	
_sed_command:	; invoke a SEDORIC command using black magic
	ldy #$0        
loop1:            ; copy command string to #35..#84
	lda _sed_fname,y
	sta $35,y
	iny
	ora #$0
	bne loop1

	sta $ea         ; update the line start pointer
	lda #$35
	sta $e9

	jsr $00e2       ; get next token
	;jsr $02f5       ; call the ! command handler
	rts
