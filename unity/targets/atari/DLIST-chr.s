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

	.export _CharmapDLIST

	.import _chrRows, _bmpRows, _bmpAddr, _chrPalette

; Char palette
colorSHADOW0 = $02c8
colorSHADOW1 = $02c4
colorSHADOW2 = $02c5
colorSHADOW3 = $02c6
colorSHADOW4 = $02c7	

	.segment	"CODE"

; ---------------------------------------------------------------
; void __near__ _CharmapDLIST (void)
; ---------------------------------------------------------------	

.proc _CharmapDLIST: near
	lda #$00		; Lower addres of DLIST $0920 -> $0900
	sta $0230	
	
header:
	lda #$70		
	sta $0900
	sta $0901
	sta $0902
	
	lda #$44		
	sta $0903
	
	lda #$50
	sta $0904
	
	lda #$09
	sta $0905
	
	ldx #0
	lda #$04
loopCHR:
	sta $0906,x
	inx
	cpx _chrRows
	bne loopCHR
	
checkSplit:	
	lda _chrRows	; Is this a split screen?
	cmp #24
	beq setupDLI2
	
setupDLI1:	
	lda #$ce		; Trigger DLI
	sta $0906,x
	inx
	
setupBMPAddr:
	lda _bmpAddr
	sta $0906,x
	inx
	lda _bmpAddr+1
	sta $0906,x
	inx
	
	lda #$0e
loopBMP:
	sta $0906,x
	inx
	cpx _bmpRows
	bne loopBMP
	
setupDLI2:	
	lda #$8e		; Trigger DLI
	sta $0906,x
	inx
	
footer:	
	lda #$41		; Footer
	sta $0906,x
	inx

	lda #$20
	sta $0906,x
	inx

	lda #$09
	sta $0906,x
	inx	
	
palette:
	; Switch to CHR palette for the top of screen
	lda _chrPalette+0
	sta  colorSHADOW0
	lda _chrPalette+1
	sta  colorSHADOW1
	lda _chrPalette+2
	sta  colorSHADOW2
	lda _chrPalette+3
	sta  colorSHADOW3
	lda _chrPalette+4
	sta  colorSHADOW4	
.endproc
