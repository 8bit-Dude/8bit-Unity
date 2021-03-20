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

	.export _BitmapDLIST

	.segment	"CODE"

; ---------------------------------------------------------------
; void __near__ _BitmapDLIST (void)
; ---------------------------------------------------------------	

.proc _BitmapDLIST: near
	lda #$00		; Lower addres of DLIST $0920 -> $0900
	sta $0230
	
	lda #$4e	; Header
	sta $0903
	lda #$10
	sta $0904
	lda #$70
	sta $0905
	
	lda #$0e
	ldx #102
loop1:
	dex
	sta $0906,x
	bne loop1
	
	lda #$4e	; Change Video Address
	sta $096b
	lda #$00
	sta $096c
	lda #$80
	sta $096d
	
	lda #$0e
	ldx #97
loop2:
	dex
	sta $096e,x
	bne loop2

	lda #$8e	; DLI
	sta $09ce
	
	lda #$41	; Footer
	sta $09cf
	lda #$20
	sta $09d0
	lda #$09
	sta $09d1
	rts
.endproc
