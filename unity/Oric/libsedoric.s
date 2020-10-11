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
;   (Original souce: OSDK)
;
	
	.export _sed_size
	.export _sed_fname
	.export _sed_begin
	.export _sed_end
	.export _sed_err
	
	.export _sed_loadfile
	.export _sed_savefile
	.export _sed_loadzp
	.export _sed_savezp
	
EXECVEC = $02f5
DOSROM  = $04f2
DOSERR  = $04fd

.code
_sed_size:  .byte 0,0
_sed_fname: .byte 0,0
_sed_begin: .byte 0,0
_sed_end:   .byte 0,0
_sed_err:   .byte 0,0
savebuf_zp: .res 256

_sed_loadfile:
	tya
	pha
	jsr _sed_savezp
	lda _sed_fname
	sta $e9
	lda _sed_fname+1
	sta $ea
	jsr DOSROM
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
	clc
	lda $c04f
	sta _sed_size
	adc _sed_begin
	sta _sed_end
	lda $c050
	sta _sed_size+1
	adc _sed_begin+1
	sta _sed_end+1
	jsr DOSROM
	jsr _sed_loadzp
	lda DOSERR
	sta _sed_err
	lda DOSERR+1
	sta _sed_err+1
	pla
	tay
	rts

_sed_savefile:
	tya
	pha
	jsr _sed_savezp
	lda _sed_fname
	sta $e9
	lda _sed_fname+1
	sta $ea
	jsr DOSROM
	lda #1
	sta $0b
	lda #$00
	sta $c018
	clc
	lda #$00
	jsr $d454
	lda _sed_begin
	sta $c052
	sta $c056
	lda _sed_begin+1
	sta $c053
	sta $c057
	lda _sed_end
	sta $c054
	lda _sed_end+1
	sta $c055
	lda #$00
	sta $c04d
	lda #$40
	sta $c04e
	lda #$40
	sta $c051
	jsr $de0b
	jsr DOSROM
	jsr _sed_loadzp
	lda DOSERR
	sta _sed_err
	lda DOSERR+1
	sta _sed_err+1
	pla
	tay
	rts

_sed_loadzp:
	ldx #00
load_loop:
	lda savebuf_zp,x
	sta $00,x
	dex
	bne load_loop
	rts

_sed_savezp:
	ldx #00
save_loop:
	lda $00,x
	sta savebuf_zp,x
	dex
	bne save_loop
	rts
