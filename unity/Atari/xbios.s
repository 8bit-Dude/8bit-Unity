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
	
	.import _enable_rom
	.import _restore_rom
	
	.export _xbios_fname
	.export _xbios_dest
	.export _xbios_len
	
	.export _xbios_list_dir
	.export _xbios_get_entry
	.export _xbios_open_file
	.export _xbios_load_data

xBIOS_OPEN_FILE   = $0509
xBIOS_LOAD_DATA   = $050c
xBIOS_SET_LENGTH  = $051e	
xBIOS_LIST_DIR    = $0512	
xBIOS_DFLT_DEVICE = $052a
xBIOS_GET_ENTRY   = $053c

_xbios_fname:  .byte 0,0
_xbios_dest:   .byte 0,0
_xbios_len:    .byte 0,0

	.segment	"CODE"

; USE FOR LOADING WITH XBIOS
;_xbios_device: .byte 0
;_xbios_set_device:
;	lda _xbios_device
;	cmp #0
;	bne done
;	jsr xBIOS_DFLT_DEVICE
;	lda #1
;	sta _xbios_device
;done:
;	rts

_xbios_list_dir:
	;jsr _xbios_set_device
	jsr _enable_rom
	jsr xBIOS_LIST_DIR
	jsr _restore_rom
	rts

_xbios_get_entry:
	jsr _enable_rom
	jsr xBIOS_GET_ENTRY
	jsr _restore_rom
	txa
	rts
	
_xbios_open_file:
	;jsr _xbios_set_device
	jsr _enable_rom
	ldy _xbios_fname
	ldx _xbios_fname+1
	jsr xBIOS_OPEN_FILE
	jsr _restore_rom
	bcs not_found
	lda #1
	rts
not_found:
	lda #0
	rts

_xbios_load_data:
	jsr _enable_rom
	ldy _xbios_len
	ldx _xbios_len+1
	jsr xBIOS_SET_LENGTH
	ldy _xbios_dest
	ldx _xbios_dest+1
	jsr xBIOS_LOAD_DATA
	jsr _restore_rom
	rts
