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

	.export _SendByte, _RecvByte
	.export _byte

	.segment	"DATA"		

_byte: .byte 0

	.segment	"CODE"		

; ---------------------------------------------------------------
; unsigned char __near__ _SendByte (void)
; ---------------------------------------------------------------	

.proc _SendByte: near

	; Wait for ready (Joy 2 / Data 4)
	ldx #255	
sendWait0:	
	lda $4017
	and #%00010000
	bne sendByte
	dex
	bne sendWait0
	jmp sendFailed
	
sendByte:

	;-----------------;
	; Write Bits 1
	lda _byte
	and #%00000001
	ora #%00000100
	sta $4016
	ldx #2
sendWait1:	
	dex
	bne sendWait1
	
	; Strobe ON/OFF
    lda $4016	
	ldx #3
sendWait2:	
	dex
	bne sendWait2
		
	;-----------------;
	; Write Bits 2-8
	ldy #7	
sendLoop:
	lda _byte
	lsr
	sta _byte
	and #%00000001
	ora #%00000100
	sta $4016
	ldx #2
sendWait3:	
	dex
	bne sendWait3
	
	; Strobe ON/OFF
    lda $4016	
	ldx #3
sendWait4:	
	dex
	bne sendWait4	
	
	; Loopback
	dey
	bne sendLoop
	
	;-----------------;
	; Turn off flags
    lda #%00000000	
	sta $4016	

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
	
	; Wait for ready (Joy 2 / Data 4)
	ldx #255	
recvWait0:	
	lda $4017
	and #%00010000
	bne recvByte
	dex
	bne recvWait0
	jmp recvFailed
	
recvByte:	

	;-----------------;
	; Turn on Recv flags
    lda #%00000110	
	sta $4016
	ldx #2
recvWait1:	
	dex
	bne recvWait1
	
	;-----------------;
	; Strobe ON/OFF
    lda $4016	
	ldx #6
sendWait2:	
	dex
	bne sendWait2	
	
	; Read Bits 1/2
	lda $4017
	and #%00001100	
	lsr
	lsr
	sta _byte
	ldx #2
sendWait3:	
	dex
	bne sendWait3		
	
	;-----------------;
	; Strobe ON/OFF
    lda $4016	
	ldx #6
sendWait4:	
	dex
	bne sendWait4	
	
	; Read Bits 3/4
	lda $4017
	and #%00001100	
	ora _byte
	sta _byte
	ldx #2
sendWait5:	
	dex
	bne sendWait5	

	;-----------------;
	; Strobe ON/OFF
    lda $4016	
	ldx #6
sendWait6:	
	dex
	bne sendWait6
	
	; Read Bits 5/6
	lda $4017
	and #%00001100	
	asl
	asl
	ora _byte
	sta _byte
	ldx #2
sendWait7:	
	dex
	bne sendWait7		

	;-----------------;
	; Strobe ON/OFF
    lda $4016	
	ldx #6
sendWait8:	
	dex
	bne sendWait8
	
	; Read Bits 7/8
	lda $4017
	and #%00001100	
	asl
	asl
	asl
	asl
	ora _byte
	sta _byte
	
	;-----------------;
	; Turn off flags
    lda #%00000000	
	sta $4016	
	
recvDone:	
	; Return 1
	lda #1
	rts

recvFailed:
	; Return 0
	lda #0
	rts

.endproc