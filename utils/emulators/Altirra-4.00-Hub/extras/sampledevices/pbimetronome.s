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

critic  = $0042

pdvmsk  = $0247
shpdvs  = $0248
pdmsk   = $0249

metronome_irqack = $D1F0
metronome_irqen = $D1F1

consol  = $d01f
vcount  = $d40b

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
        tax
        ora     pdvmsk          ;merge into enabled mask
        sta     pdvmsk
        txa
        ora     pdmsk           ;merge into IRQ mask
        sta     pdmsk

        lda     #1
        sta     metronome_irqen ;enable metronome IRQ
        rts
.endp

;==========================================================================
.proc SIOHandler
        clc                     ;not handled
        rts
.endp

;==========================================================================
.proc IRQHandler
        ;ack the IRQ
        sta     metronome_irqack

        ;if CRITIC is set, we're in a disk I/O operation and it would be
        ;unwise to run
        lda     critic
        bne     skip_tone

        ;drop to passive level
        cli

        ldx     #32
tone_loop:
        lda     vcount
        cmp:req vcount

        txa
        and     #8
        sta     consol
        dex
        bne     tone_loop

skip_tone:
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
