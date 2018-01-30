
	.export _PlayMusic
	.export _StopMusic

sid_init = $b048
sid_play = $b021

	.segment "CODE"

; ---------------------------------------------------------------
; void __near__ _PlayMusic (void)
; void __near__ _StopMusic (void)
; ---------------------------------------------------------------

.proc _PlayMusic: near
		; init music
        jsr sid_init
		
        ; set IRQ pointer in $314/$315
        sei
        lda #<InterruptSID
        ldx #>InterruptSID
        sta $314
        stx $315
        cli 

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
		; set through tune
        jsr sid_play
		
		; do the normal interrupt service routine		
        jmp $EA31 

