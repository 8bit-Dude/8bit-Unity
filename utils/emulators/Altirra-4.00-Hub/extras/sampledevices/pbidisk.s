; Altirra sample custom device sample - PBI disk firmware
; Copyright (C) 2020 Avery Lee, All rights reserved.
;
; This software is provided 'as-is', without any express or implied
; warranty.  In no event will the authors be held liable for any
; damages arising from the use of this software.
; 
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
; 
; 1. The origin of this software must not be misrepresented; you must
;    not claim that you wrote the original software. If you use this
;    software in a product, an acknowledgment in the product
;    documentation would be appreciated but is not required.
; 2. Altered source versions must be plainly marked as such, and must
;    not be misrepresented as being the original software.
; 3. This notice may not be removed or altered from any source
;    distribution.

bufrlo  = $0032
bufrhi  = $0033
bfenlo  = $0034
bfenhi  = $0035

ddevic  = $0300
dunit   = $0301
dcomnd  = $0302
dstats  = $0303
dbuflo  = $0304
dbufhi  = $0305
dtimlo  = $0306
dbytlo  = $0308
dbythi  = $0309
daux1   = $030a
daux2   = $030b
pdvmsk  = $0247
shpdvs  = $0248

        org     $D800
        opt     h-f+

        dta     a(0)            ;checksum (unused)
        dta     0               ;revision (unused)
        dta     $80             ;ID byte
        dta     0               ;device type (unused)
        jmp     SIOHandler      ;SIO vector
        jmp     IRQHandler      ;IRQ vector
        dta     $91             ;ID byte
        dta     0               ;device name (unused)
        dta     a(CIOOpen)
        dta     a(CIOClose)
        dta     a(CIOGetByte)
        dta     a(CIOPutByte)
        dta     a(CIOGetStatus)
        dta     a(CIOSpecial)
        jmp     Init

;==========================================================================
.proc Init
        lda     shpdvs          ;get current mask
        ora     pdvmsk          ;merge into enabled mask
        sta     pdvmsk
        rts
.endp

;==========================================================================
.proc SIOHandler
        lda     ddevic          ;device ID
        clc                     ;
        adc     dunit           ;device ID + unit
        sbc     #$31            ;device ID + unit - 1 == disk 1 ID?
        beq     is_disk1        ;jump if it's disk 1
        clc                     ;not handled
        rts

is_disk1:
        lda     dcomnd
        cmp     #$50            ;check if put sector
        beq     is_write
        cmp     #$57            ;check if write sector
        beq     is_write
        cmp     #$21            ;check if format
        bne     not_write
is_write:
        ;write command - return device error
        ldy     #$90
        sty     dstats
        sec
        rts

not_write:
        cmp     #$53            ;check if status
        bne     not_status
        jmp     DiskStatus
not_status:

        cmp     #$52            ;check if read
        bne     not_read
        jmp     DiskRead

not_read:
        ldy     #$8b            ;NAK the command
        sty     dstats
        sec
        rts
.endp

;==========================================================================
.proc DiskStatus
        ;status command
        lda     #4
        jsr     CheckTransferLength

        bit     dstats
        bvc     skip_transfer

        ldy     #3
        mva:rpl status_vals,y (bufrlo),y-

skip_transfer:
        ldy     #$01
        sty     dstats
        sec
        rts

status_vals:
        dta     $08
        dta     $FF
        dta     $E0
        dta     $00
.endp

;==========================================================================
.proc DiskRead
        ;set up for a 128 byte transfer
        lda     #$80
        jsr     CheckTransferLength

        ;validate the sector number
        ldy     daux2
        ldx     daux1
        bne     sec_nonzero
        tya
        bne     sec_nonzero

sec_invalid:
        ldy     #$8b            ;NAK command
        sty     dstats
        sec
        rts

sec_nonzero:
        txa
        sne:dey
        dex

        cpy     #>720
        bcc     sec_ok
        bne     sec_invalid
        cpx     #<720
        bcs     sec_invalid
sec_ok:
        ;convert sector number to bank address and switch banks
        tya
        lsr
        sta     $d601       ;set high bank (bit 8)
        txa
        ror
        sta     $d600       ;set low bank (bits 0-7)
        lda     #0
        ror
        sta     bfenlo      ;select top or bottom half of bank
        lda     #$d6
        sta     bfenhi

        ;copy from ROM to transfer buffer
        ldy     #$7f
        mva:rpl (bfenlo),y (bufrlo),y-

        ;all done
        ldy     #$01
        sty     dstats
        sec
        rts
.endp

;==========================================================================
.proc CheckTransferLength
        ldx     dbythi
        bne     too_long
        cmp     dbytlo
        bne     mismatch

        ;all good - set up transfer pointer
        mwa     dbuflo bufrlo
        rts

mismatch:
        bcs     too_long

        ;transfer length too short - return checksum error
        ldy     #$8f
        sec
        pla
        pla
        rts

too_long:
        ;transfer length too long - return timeout
        ldy     #$8a
        sec
        pla
        pla
        rts
.endp

;==========================================================================
.proc IRQHandler
        rts
.endp

;==========================================================================
CIOOpen:
CIOClose:
CIOGetByte:
CIOPutByte:
CIOGetStatus:
CIOSpecial:
        rts

;==========================================================================

        org     $DFFF
        dta     0
