;
; Copyright (c) 2018 Anthony Beaucamp.
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

	.export _PlayMusic
	.export _StopMusic

sid_address: .res 2

	.segment "CODE"

; ---------------------------------------------------------------
; void __near__ _PlayMusic (unsigned int address)
; void __near__ _StopMusic (void)
; ---------------------------------------------------------------

.proc _PlayMusic: near
		; set JSR addresses
		stx checkSID+2
		stx initSID+2
		stx InterruptSID+2
		
checkSID:
		; check SID actually exists
		lda $0048
		cmp #0
		beq skipSID

initSID:
		; init code (00 replaced by low-byte of SID address)
		jsr $0048
		
        ; set IRQ pointer in $314/$315
        sei
        lda #<InterruptSID
        ldx #>InterruptSID
        sta $314
        stx $315
        cli
		
skipSID:		
        rts
.endproc        

.proc _StopMusic: near
		; restore IRQ vector to kernel interrupt routine
        sei
		ldx #$31
		ldy #$ea
		stx $314
		sty $315
        cli 
		
		; reset SID state
        ldx     #$18
        lda     #$00
rst1:    
        sta     $d400,x
        dex
        bpl     rst1
 
        lda     #$08
        sta     $d404
        sta     $d40b
        sta     $d412 
        ldx    #$03
rst2:       
        bit     $d011
        bpl     *-3
        bit     $d011
        bmi     *-3
        dex
        bpl     rst2
 
        lda     #$00
        sta     $d404
        sta     $d40b
        sta     $d412
        lda     #$00
        sta     $d418
		
        rts 
.endproc 


InterruptSID:
		; interrupt code (00 replaced by low-byte of SID address)
        jsr $0021
		
		; do the normal interrupt service routine		
        jmp $EA31 

