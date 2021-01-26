;
; Copyright (c) 2020 Anthony Beaucamp.
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

	.export _hiresLinesHI, _hiresLinesLO

	.export _HiresLine
	
	.segment	"DATA"
		
_hiresLinesHI: 
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
 	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
 	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $20,$24,$28,$2C,$30,$34,$38,$3C
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $21,$25,$29,$2D,$31,$35,$39,$3D
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
	.byte $22,$26,$2A,$2E,$32,$36,$3A,$3E
 	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	.byte $23,$27,$2B,$2F,$33,$37,$3B,$3F
	
_hiresLinesLO: 
	.byte $00,$00,$00,$00,$00,$00,$00,$00
 	.byte $80,$80,$80,$80,$80,$80,$80,$80
	.byte $00,$00,$00,$00,$00,$00,$00,$00
 	.byte $80,$80,$80,$80,$80,$80,$80,$80
	.byte $00,$00,$00,$00,$00,$00,$00,$00
 	.byte $80,$80,$80,$80,$80,$80,$80,$80
	.byte $00,$00,$00,$00,$00,$00,$00,$00
 	.byte $80,$80,$80,$80,$80,$80,$80,$80
 	.byte $28,$28,$28,$28,$28,$28,$28,$28
 	.byte $A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8
 	.byte $28,$28,$28,$28,$28,$28,$28,$28
 	.byte $A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8
 	.byte $28,$28,$28,$28,$28,$28,$28,$28
 	.byte $A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8
 	.byte $28,$28,$28,$28,$28,$28,$28,$28
 	.byte $A8,$A8,$A8,$A8,$A8,$A8,$A8,$A8
 	.byte $50,$50,$50,$50,$50,$50,$50,$50
 	.byte $D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0
 	.byte $50,$50,$50,$50,$50,$50,$50,$50
 	.byte $D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0
 	.byte $50,$50,$50,$50,$50,$50,$50,$50
 	.byte $D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0
 	.byte $50,$50,$50,$50,$50,$50,$50,$50
 	.byte $D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0

	.segment	"LOWCODE"

; ---------------------------------------------------------------
; int __near__ HiresLine (char line)
;	Return address of Hires line
; ---------------------------------------------------------------	
	
.proc _HiresLine: near

	; Save Hires address to registers A/X
	tay
	ldx _hiresLinesHI,y
	lda _hiresLinesLO,y
	rts
.endproc	
