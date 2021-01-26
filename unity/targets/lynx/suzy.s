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
; Adapted from TGI driver by Karri Kaksonen
;

	.include "lynx.inc"

	.import __viddma: zp

	.export _SuzyInit
	.export _SuzyFlip
	.export _SuzyDraw
	.export _SuzyUpdate
	.export _SuzyBusy

	.global __sprsys: zp
		
	.interruptor VBLIRQ

	.segment "DATA"	

VBLIRQ:    .byte   $60, $00, $00       ; RTS plus two dummy bytes

pixel:    .byte   3,%10000100,%00000000, $0       ; A pixel bitmap (used for clear screen)
cls_coll: .byte   0
cls_sprite:
         .byte   %00000001                        ; A pixel sprite (used for clear screen)
         .byte   %00010000
         .byte   %00100000
         .addr   0,pixel
         .word   0
         .word   0
         .word   $a000                           ; 160
         .word   $6600                           ; 102
         .byte   $00
		 
	.segment "BSS"	

; Double buffer IRQ stuff
DRAWPAGE:      .res 1
SWAPREQUEST:   .res 1
_DrawPageAddr: .res 2

	.segment "CODE"	

; ---------------------------------------------------------------
; void __near__ SuzyInit (void)
; ---------------------------------------------------------------	
	
_SuzyInit:
; Init parameters
	lda     #1
	stz     DRAWPAGE
	stz     SWAPREQUEST
	jsr 	SETDRAWPAGE
; Enable interrupts for VBL
	lda 	#$80
	tsb 	VTIMCTLA
; Set up collision buffer to $A058
	lda 	#$58
	sta 	COLLBASL
	lda 	#$A0
	sta 	COLLBASH
; Put collision index before sprite data
	lda 	#$FF
	sta 	COLLOFFL
	lda 	#$FF
	sta 	COLLOFFH
; Set framerate to 60 fps		
	lda 	#$9e            ; 60 Hz
	ldx 	#$29
	sta 	HTIMBKUP
	stx 	PBKUP
; Install vertical blank interrupt
	lda 	#<IRQ
	sta 	VBLIRQ+1
	lda 	#>IRQ
	sta 	VBLIRQ+2
	lda     #$4C        ; Jump opcode
	sta     VBLIRQ      ; Activate IRQ routine
; Finally, clear screen by drawing black pixel
	lda     #<cls_sprite
    ldx     #>cls_sprite
    jsr     _SuzyDraw	
	jsr     _SuzyUpdate
	rts	

; ---------------------------------------------------------------
; void __near__ SuzyFlip (void* ptr)
; ---------------------------------------------------------------	

_SuzyFlip:
; Flip screen
	lda     __sprsys
	eor     #8
	sta     __sprsys
	sta     SPRSYS
	lda     __viddma
	eor     #2
	sta     __viddma
	sta     DISPCTL
	rts

; ---------------------------------------------------------------
; void __near__ SuzyDraw (void* ptr)
; ---------------------------------------------------------------	

_SuzyDraw:
; Draw in render buffer
	sta     SCBNEXTL
	stx     SCBNEXTH	
	lda     _DrawPageAddr
	ldx     _DrawPageAddr+1
	sta     VIDBASL
	stx     VIDBASH
	lda     #1
	sta     SPRGO
	stz     SDONEACK
@L0:    
	stz     CPUSLEEP
	lda     SPRSYS
	lsr
	bcs     @L0
	stz     SDONEACK
	rts		

; ---------------------------------------------------------------
; void __near__ SuzyUpdate (unsigned char swap)
; ---------------------------------------------------------------	
	
.proc _SuzyUpdate: near
	lda 	#1
	sta     SWAPREQUEST
	rts
.endproc		

; ---------------------------------------------------------------
; unsigned char __near__ SuzyBusy (void)
; ---------------------------------------------------------------	

.proc _SuzyBusy: near
	lda     SWAPREQUEST
	rts
.endproc

; ------------------------------------------------------------------------
; SETVIEWPAGE: Set the visible page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
;
; It is a good idea to call this function during the vertical blanking
; period. If you call it in the middle of the screen update then half of
; the drawn frame will be from the old buffer and the other half is
; from the new buffer. This is usually noticed by the user.
; ---------------------------------------------------------------	

SETVIEWPAGE:
        cmp     #1
        beq     @L1             ; page == maxpages-1
        ldy     #<$e018         ; page 0
        ldx     #>$e018
        bra     @L2
@L1:
        ldy     #<$c038         ; page 1
        ldx     #>$c038
@L2:
        lda     __viddma        ; Process flipped displays
        and     #2
        beq     @L3
        clc
        tya
        adc     #<8159
        tay
        txa
        adc     #>8159
        tax
@L3:
        sty     DISPADRL        ; $FD94
        stx     DISPADRH        ; $FD95
        rts

; ------------------------------------------------------------------------
; SETDRAWPAGE: Set the drawable page. Called with the new page in A (0..n).
; The page number is already checked to be valid by the graphics kernel.
; ---------------------------------------------------------------	

SETDRAWPAGE:
        cmp     #1
        beq     @L1                 ; page == maxpages-1
        lda     #<$e018             ; page 0
        ldx     #>$e018
        bra     @L2
@L1:
        lda     #<$c038             ; page 1
        ldx     #>$c038
@L2:
        sta     _DrawPageAddr
        stx     _DrawPageAddr+1
        rts

; ------------------------------------------------------------------------
; IRQ: VBL interrupt handler
; ---------------------------------------------------------------	

IRQ:
        lda     INTSET          ; Poll all pending interrupts
        and     #VBL_INTERRUPT
        beq     IRQEND          ; Exit if not a VBL interrupt

        lda     SWAPREQUEST
        beq     @L0
        lda     DRAWPAGE
        jsr     SETVIEWPAGE
        lda     DRAWPAGE
        eor     #1
        sta     DRAWPAGE
        jsr     SETDRAWPAGE
        stz     SWAPREQUEST
@L0:
IRQEND:
        clc
        rts
