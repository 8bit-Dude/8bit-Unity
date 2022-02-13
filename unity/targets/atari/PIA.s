;
; Copyright (c) 2021 Anthony Beaucamp.
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

	.export _OutputMode, _InputMode, _SendByte, _RecvByte
	.export _byte

	.segment	"DATA"		

_byte: .byte 0

	.segment	"CODE"		
	
; ---------------------------------------------------------------
; void __near__ _OutputMode (void)
; ---------------------------------------------------------------	

.proc _OutputMode: near	

    lda #%00111000	
	sta $D302			

    lda #%11100000	; Set PIA State (Strobe, Bit Out, R/W, Ready)
	sta $D300			

    lda #%00001100	
	sta $D302			

    lda #%10000000	; Strobe On, Write Mode
	sta $D300			

	rts

.endproc

; ---------------------------------------------------------------
; void __near__ _InputMode (void)
; ---------------------------------------------------------------	

.proc _InputMode: near	

    lda #%00111000	
	sta $D302			

    lda #%10100000	; Set PIA State (Strobe, Bit In, R/W, Ready)
	sta $D300			

    lda #%00001100	
	sta $D302			

    lda #%10100000	; Strobe On, Read Mode
	sta $D300			

	rts

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ _SendByte (void)
; ---------------------------------------------------------------	

.proc _SendByte: near

	; Wait for ready (Pin 4)
	ldx #255	
sendWait0:	
	lda $D300
	and #%00010000	
	bne sendByte
	dex
	bne sendWait0
	jmp sendFailed
	
	;------------;
	; Write Bits 
sendByte:
	lda _byte
	lsr
	ldy #8
	
sendLoop:
	and #%01000000
	sta $D300
	ldx #8
sendWait1:	
	dex
	bne sendWait1
	
	; Strobe ON
    ora #%10000000	
	sta $D300
	ldx #4
sendWait2:
	dex
	bne sendWait2
	
	; Prepare next byte
	lda _byte
	asl
	sta _byte
	lsr

	; Loopback
	dey
	bne sendLoop	
	
sendDone:	
	lda #1
	rts

sendFailed:
	lda #0
	rts

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ _RecvByte (void)
; ---------------------------------------------------------------	

.proc _RecvByte: near
	
	; Wait for ready (Pin 4)
	ldx #255	
recvWait0:	
	lda $D300
	and #%00010000	
	bne recvByte
	dex
	bne recvWait0
	jmp recvFailed
	
	;--------------;
	; Receive Bits
recvByte:	
	lda #0
	sta _byte
	ldy #8
	
recvLoop:	
	; Strobe OFF
    lda #%00100000	
	sta $D300
	ldx #6
recvWait1:	
	dex
	bne recvWait1

	; Strobe ON
    lda #%10100000	
	sta $D300

	; Shift data
	lsr _byte
	
	; Read Bit
	lda $D300
	and #%01000000
	asl
	ora _byte
	sta _byte
	
	; Loopback
	dey
	bne recvLoop		
		
recvDone:	
	lda #1
	rts

recvFailed:
	lda #0
	rts

.endproc