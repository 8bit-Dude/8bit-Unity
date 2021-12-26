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

    lda #%11110000	; Set PIA State
	sta $D300			

    lda #%00001100	
	sta $D302			

    lda #%10000000	; Strobe ON
	sta $D300			

	rts

.endproc

; ---------------------------------------------------------------
; void __near__ _InputMode (void)
; ---------------------------------------------------------------	

.proc _InputMode: near	

    lda #%00111000	
	sta $D302			

    lda #%11000000	; Set PIA State
	sta $D300			

    lda #%00001100	
	sta $D302			

    lda #%11000000	; Strobe ON
	sta $D300			

	rts

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ _SendByte (void)
; ---------------------------------------------------------------	

.proc _SendByte: near

	; Wait for ready (Joy1 Trigger)
	ldx #255	
sendWait0:	
	lda $D011		
	beq sendByte
	dex
	bne sendWait0
	jmp sendFailed
	
sendByte:

	;-----------------;
	; Write Bits 1/2
	lda _byte
	and #%00000011	
	asl
	asl
	asl
	asl
	sta $D300
	ldx #9
sendWait1:	
	dex
	bne sendWait1
	
	; Strobe ON
    lda #%10000000	
	sta $D300
	ldx #3
sendWait2:	
	dex
	bne sendWait2
	
	;-----------------;
	; Write Bits 3/4
	lda _byte
	and #%00001100	
	asl
	asl
	sta $D300
	ldx #9
sendWait3:	
	dex
	bne sendWait3

	; Strobe ON
    lda #%10000000	
	sta $D300
	ldx #3
sendWait4:	
	dex
	bne sendWait4

	;-----------------;
	; Write Bits 5/6
	lda _byte
	and #%00110000	
	sta $D300
	ldx #9
sendWait5:	
	dex
	bne sendWait5
	
	; Strobe ON
    lda #%10000000	
	sta $D300
	ldx #3
sendWait6:	
	dex
	bne sendWait6	
	
	;-----------------;
	; Write Bits 7/8
	lda _byte
	and #%11000000	
	lsr
	lsr
	sta $D300
	ldx #9
sendWait7:	
	dex
	bne sendWait7
	
	; Strobe ON
    lda #%10000000	
	sta $D300
	ldx #3
sendWait8:	
	dex
	bne sendWait8	

sendDone:	
	; Return 1
	lda #1
	rts

sendFailed:
	; Return 0
	lda #0
	rts

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ _RecvByte (void)
; ---------------------------------------------------------------	

.proc _RecvByte: near
	
	; Wait for ready (Joy1 Trigger)
	ldx #255
recvWait0:	
	lda $D011		
	beq recvByte
	dex
	bne recvWait0
	jmp recvFailed
	
recvByte:	
	
	;-----------------;
	; Strobe OFF
    lda #%01000000	
	sta $D300
	ldx #6
recvWait1:	
	dex
	bne recvWait1
	
	; Read Bits 1/2
	lda $D300
	and #%00110000	
	lsr
	lsr
	lsr
	lsr
	sta _byte
	
	; Strobe ON
    lda #%11000000	
	sta $D300
	ldx #3
recvWait2:	
	dex
	bne recvWait2	
	
	;-----------------;
	; Strobe OFF
    lda #%01000000	
	sta $D300
	ldx #6
recvWait3:	
	dex
	bne recvWait3
	
	; Read Bits 3/4
	lda $D300
	and #%00110000	
	lsr
	lsr
	ora _byte
	sta _byte
	
	; Strobe ON
    lda #%11000000	
	sta $D300
	ldx #3
recvWait4:	
	dex
	bne recvWait4	
	
	;-----------------;
	; Strobe OFF
    lda #%01000000	
	sta $D300
	ldx #6
recvWait5:	
	dex
	bne recvWait5
	
	; Read Bits 5/6
	lda $D300
	and #%00110000	
	ora _byte
	sta _byte
	
	; Strobe ON
    lda #%11000000	
	sta $D300
	ldx #3
recvWait6:	
	dex
	bne recvWait6	
	
	;-----------------;
	; Strobe OFF
    lda #%01000000	
	sta $D300
	ldx #6
recvWait7:	
	dex
	bne recvWait7
	
	; Read Bits 5/6
	lda $D300
	and #%00110000	
	asl
	asl
	ora _byte
	sta _byte
	
	; Strobe ON
    lda #%11000000	
	sta $D300
	ldx #3
recvWait8:	
	dex
	bne recvWait8	
	
recvDone:	
	; Return 1
	lda #1
	rts

recvFailed:
	; Return 0
	lda #0
	rts

.endproc