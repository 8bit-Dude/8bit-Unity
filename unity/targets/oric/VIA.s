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

	.export _OutputMode, _InputMode, _ClosePort, _SendByte, _RecvByte
	.export _byte

	.segment	"DATA"		

_byte: .byte 0

	.segment	"CODE"		
	
; ---------------------------------------------------------------
; void __near__ _OutputMode (void)
; ---------------------------------------------------------------	

.proc _OutputMode: near	

	lda #%11111111	;  Set port A as Output	
	sta $0303			
	
	lda #%10000000  ;  Ready to Send
	sta $0301	

	rts

.endproc

; ---------------------------------------------------------------
; void __near__ _InputMode (void)
; ---------------------------------------------------------------	

.proc _InputMode: near	
	
	lda #%11110000	;  Set port A as Input	
	sta $0303	
	
	lda #%11000000  ;  Ready to Receive
	sta $0301	

	lda $0301		;  Reset ORA
					
	rts

.endproc

; ---------------------------------------------------------------
; void __near__ _ClosePort (void)
; ---------------------------------------------------------------	

.proc _ClosePort: near	

	lda #%11111111	;  Set port A as Output	
	sta $0303			
	
	lda #%00000000  ;  Unavailable
	sta $0301	
	
	; Strobe OFF (to advertise the change)
	lda $0300
    and #%11101111	
	sta $0300
	
	; Strobe ON
    ora #%00010000	
	sta $0300	

	rts

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ _SendByte (void)
; ---------------------------------------------------------------	

.proc _SendByte: near
		
sendByte1:		
	;----------------------------
	; Write Bits 1-4 + STROBE OFF
	lda _byte
	and #%00001111	
	ora #%10000000	
	sta $0301

	; Strobe OFF
	lda $0300
    and #%11101111	
	sta $0300
	
	; Strobe ON
    ora #%00010000	
	sta $0300

	; Wait for ACKNOW
	ldx #255	
sendWait0:	
	lda $030d
	and #%00000010
	bne sendByte2
	dex
	bne sendWait0
	jmp sendFailed	
	
sendByte2:
	;----------------------------
	; Write Bits 5-8 + STROBE OFF
	lda _byte
	and #%11110000	
	lsr
	lsr
	lsr
	lsr
	ora #%10000000	
	sta $0301

	; Strobe OFF
	lda $0300
    and #%11101111	
	sta $0300
	
	; Strobe ON
    ora #%00010000	
	sta $0300
	
	; Wait for ACKNOW
	ldx #255	
sendWait1:	
	lda $030d
	and #%00000010
	bne sendDone
	dex
	bne sendWait1
	jmp sendFailed	
	
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
	
recvByte:

	;-----------------;
	; Strobe OFF
	lda $0300
    and #%11101111	
	sta $0300
	
	; Strobe ON
    ora #%00010000	
	sta $0300
	
	; Wait for ACKNOW
	ldx #255	
recvWait0:	
	lda $030d
	and #%00000010
	bne recvByte1
	dex
	bne recvWait0
	jmp recvFailed		

recvByte1:	
	; Read Bits 1-4
	lda $0301
	and #%00001111
	sta _byte
	
	;-----------------;
	; Strobe OFF
	lda $0300
    and #%11101111	
	sta $0300
	
	; Strobe ON
    ora #%00010000	
	sta $0300
	
	; Wait for ACKNOW
	ldx #255	
recvWait1:	
	lda $030d
	and #%00000010
	bne recvByte2
	dex
	bne recvWait1
	jmp recvFailed		

recvByte2:		
	; Read Bits 5-8
	lda $0301
	and #%00001111
	asl
	asl
	asl
	asl
	ora _byte
	sta _byte
	
recvDone:	
	; Return 1
	lda #1
	rts

recvFailed:
	; Return 0
	lda #0
	rts
		
.endproc