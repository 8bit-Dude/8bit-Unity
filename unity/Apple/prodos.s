;
; Copyright (c) 2020 Anthony Beaucamp.
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
	
	.export _prodos_fname
	.export _prodos_dest
	.export _prodos_len
	
	.export _prodos_open_file
	.export _prodos_load_data

_prodos_flen:  .res 1
_prodos_fname: .res 13
_prodos_dest:  .res 2
_prodos_len:   .res 2

_params: .res 4

_prodos_open_file:
	lda #$03			; 3 params (pointer to filename, pointer to I/O buffer (unused), ref. number (result))
    sta _params+0
	lda #<_prodos_flen  ; low byte of len+name address
    sta _params+1
    lda #>_prodos_flen  ; high byte of len+name address
    sta _params+2
    lda #$00			; use file 0
    sta _params+3
    lda #$A6			; use file 0
    sta _params+4       
    lda #$C8            ; open command
    sta _mli+3
    jsr _mli            ; open the file
	rts

_prodos_load_data:
	lda #$03			; 4 params (file ref. number, pointer to buffer, num of bytes, number actually read)
    sta _params+0
	lda #$00			; use file 0
    sta _params+1
	lda #<_prodos_dest  ; low byte of buffer address
    sta _params+2
    lda #>_prodos_dest  ; high byte of buffer address
    sta _params+3
	lda _prodos_len+0 	; low byte of read num
    sta _params+4
    lda _prodos_len+1  	; high byte of read num
    sta _params+5
    lda #$CA            ; read command
    sta _mli+3
    jsr _mli            ; open the file
	rts
	
_mli: 
	jsr $BF00       	; call ProDOS
    .byte $00          	; command number
    .word _params       ; address of parameter table
    rts
	