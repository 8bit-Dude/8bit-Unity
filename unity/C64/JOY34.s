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

	.export _InitJoy34
	.export _GetJoy3
	.export _GetJoy4

; C64/C128
USERPORT_DATA = $DD01
USERPORT_DDR  = $DD03

; VIC20
;USERPORT_DATA = $9110
;USERPORT_DDR  = $9112

; PET
;USERPORT_DATA = $E841
;USERPORT_DDR  = $E843

; CBM610
;USERPORT_DATA = $DC01
;USERPORT_DDR  = $DC03

_joy4:
	.res 1,$03	
	
	.segment	"CODE"	
	
;---------------------------------------
; initialize CIAs
;---------------------------------------
;warning: do not mess around with this
;         unless you really know what
;         you are doing! wrong cia setup
;         may toast your cia !
;---------------------------------------

.proc _InitJoy34: near
        lda #%10000000          ; cia 2 port B Data-Direction
        sta USERPORT_DDR        ; bit 7: out    bit 6-0: in
        lda #0
        sta USERPORT_DATA       ; output zero at PB7
		rts
.endproc

;---------------------------------------
; Read adapter and composes the
; additional virtual joystick-registers
; ---------------------------------------------------------------
; unsigned char __near__ getJoy3 (void)
; unsigned char __near__ getJoy4 (void)
; ---------------------------------------------------------------

.proc _GetJoy3: near
	lda #$80                ; cia 2 port B read/write
	sta USERPORT_DATA       ; (output one at PB7)
	lda USERPORT_DATA       ; cia 2 port B read/write
	and #$1f                ; get bit 4-0 (PB4-PB0)
	rts
.endproc

.proc _GetJoy4: near
	lda #$00                ; cia 2 port B read/write
	sta USERPORT_DATA       ; (output zero at PB7)
	lda USERPORT_DATA       ; cia 2 port B read/write
	and #$0f                ; get bit 3-0 (PB3-PB0)
	sta _joy4               ; joy 4 directions
	lda USERPORT_DATA       ; cia 2 port B read/write
	and #%00100000          ; get bit 5 (PB5)
	lsr
	ora _joy4				; joy 4 button
	rts
.endproc
