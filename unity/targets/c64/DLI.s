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

	.export _StartDLI, _StopDLI
	.export _rasterLine

	.segment "DATA"	

_rasterLine: .byte 0

	.segment "CODE"	
	
; ---------------------------------------------------------------
; void __near__ _StartDLI (void)
; ---------------------------------------------------------------	

.proc _StartDLI: near
	SEI                  ; set interrupt bit, make the CPU ignore interrupt requests
	LDA #%01111111       ; switch off interrupt signals from CIA-1
	STA $DC0D
	
	AND $D011            ; clear most significant bit of VIC's raster register
	STA $D011
	
	LDA $DC0D            ; acknowledge pending interrupts from CIA-1
	LDA $DD0D            ; acknowledge pending interrupts from CIA-2
	
	LDA _rasterLine         
	STA $D012			 ; set rasterline where first interrupt shall occur
	
	LDA #<Irq1           ; set interrupt vectors, pointing to interrupt service routine below
	STA $0314
	LDA #>Irq1
	STA $0315

	LDA #%00000001       ; enable raster interrupt signals from VIC
	STA $D01A

	CLI                  ; clear interrupt flag, allowing the CPU to respond to interrupt requests
	RTS
.endproc

; ---------------------------------------------------------------
; void __near__ _StopDLI (void)
; ---------------------------------------------------------------	

.proc _StopDLI: near
	SEI                  ; set interrupt bit, make the CPU ignore interrupt requests
	LDA #%00000000       ; disable raster interrupt signals from VIC
	STA $D01A	
	CLI                  ; clear interrupt flag, allowing the CPU to respond to interrupt requests
	RTS
.endproc

;-----------------------------------------
Irq1:
	LDA $D011             ; select bitmap screen mode
	ORA #%00100000
	STA $D011

	LDA #<Irq2            ; set interrupt vectors to the second interrupt service routine at Irq2
	STA $0314
	LDA #>Irq2
	STA $0315

	LDA #0
	STA $D012            ; next interrupt will occur at line no. 0

	ASL $D019            ; acknowledge the interrupt by clearing the VIC's interrupt flag
	JMP $EA31            ; jump into KERNAL's standard interrupt service routine to handle keyboard scan, cursor display etc.

;-----------------------------------------
Irq2:
	LDA $D011             ; select text screen mode
	AND #%11011111
	STA $D011

	LDA #<Irq1             ; set interrupt vectors back to the first interrupt service routine at Irq
	STA $0314
	LDA #>Irq1
	STA $0315

	LDA _rasterLine
	STA $D012            ; next interrupt will occur at rasterLine

	ASL $D019            ; acknowledge the interrupt by clearing the VIC's interrupt flag
	JMP $EA81            ; jump into shorter ROM routine to only restore registers from the stack etc
