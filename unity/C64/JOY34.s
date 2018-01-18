
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

_joy3:
	.res 1,$02
_joy4:
	.res 1,$03	

	.export _initJoy34
	.export _updateJoy34
	.export _getJoy3		
	.export _getJoy4		
	
.segment	"CODE"	
	
.proc _initJoy34: near
;---------------------------------------
; initialize CIAs
;---------------------------------------
;warning: do not mess around with this
;         unless you really know what
;         you are doing! wrong cia setup
;         may toast your cia !
;---------------------------------------

        lda #%10000000          ; cia 2 port B Data-Direction
        sta USERPORT_DDR        ; bit 7: out    bit 6-0: in
        lda #0
        sta USERPORT_DATA       ; output zero at PB7
		rts
.endproc

.proc _updateJoy34: near
;---------------------------------------
; reads adapter and composes the
; additional virtual joystick-registers
;---------------------------------------
;this code demonstrates how to read the
;additional 2 joysticks and how to com-
;pose 2 'virtual' joystick-registers
;that can be processed exactly like the
;usual ($dc00/$dc01) ones.
;---------------------------------------

        lda #$80                ; cia 2 port B read/write
        sta USERPORT_DATA       ; (output one at PB7)
        lda USERPORT_DATA       ; cia 2 port B read/write
        and #$1f                ; get bit 4-0 (PB4-PB0)
        sta _joy3               ; joy 3 complete
        lda #$00                ; cia 2 port B read/write
        sta USERPORT_DATA       ; (output zero at PB7)
        lda USERPORT_DATA       ; cia 2 port B read/write
        and #$0f                ; get bit 3-0 (PB3-PB0)
        sta _joy4               ; joy 4 directions
        lda USERPORT_DATA       ; cia 2 port B read/write
        and #%00100000          ; get bit 5 (PB5)
        lsr
        ora _joy4
        sta _joy4               ; joy 4 button
		rts
.endproc

; ---------------------------------------------------------------
; unsigned char __near__ getJoy3 (void)
; unsigned char __near__ getJoy4 (void)
; ---------------------------------------------------------------

.proc _getJoy3: near
	lda     _joy3
	rts
.endproc

.proc _getJoy4: near
	lda     _joy4
	rts
.endproc
