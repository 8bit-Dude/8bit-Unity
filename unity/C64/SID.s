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
	
	.export _musicPaused
	.export _sidInitAddr
	.export _sidPlayAddr

	.segment "DATA"	

_musicPaused: .byte 0
_sidInitAddr: .byte $09,03		
_sidPlayAddr: .byte $08,06	

	.segment "CODE"	

; ---------------------------------------------------------------
; void __near__ _PlayMusic (void)
; ---------------------------------------------------------------

_PlayMusic:
		; set JSR addresses
		;lda _sidInitAddr
		;sta  checkSID+3
		;lda _sidInitAddr+1
		;sta  checkSID+2
		
checkSID:
		; check SID actually exists
		;clc
		;lda $0903
		;cmp #$A0
		;bne skipSID

copyAddr:
		; set JSR addresses
		lda _sidInitAddr
		sta  initSID+2
		lda _sidInitAddr+1
		sta  initSID+1
		
		lda _sidPlayAddr
		sta  interruptSID+2
		lda _sidPlayAddr+1
		sta  interruptSID+1		
		

initSID:
		; init code	(default address is $0903)
		jsr $0903
		
        ; set IRQ pointer in $314/$315
        sei
        lda #<interruptSID
        ldx #>interruptSID
        sta $314
        stx $315
        cli
		
skipSID:		
        rts       

; ---------------------------------------------------------------
; void __near__ _StopMusic (void)
; ---------------------------------------------------------------

_StopMusic:
		; restore IRQ vector to kernel interrupt routine
        sei
		ldx #$31
		ldy #$ea
		stx $314
		sty $315
        cli 
		
		jsr resetSID
        rts 

; ---------------------------------------------------------------

interruptSID:
		; Check if track is paused
		;lda _musicPaused
		;beq playFrame
		
		; Stop sound for now
		;jsr resetSID
		;jmp doneFrame
	
playFrame:	
		; interrupt code (default address is $0806)
        jsr $0806
		
doneFrame:
		; do the normal interrupt service routine		
        jmp $EA31 

; ---------------------------------------------------------------

resetSID:
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
		