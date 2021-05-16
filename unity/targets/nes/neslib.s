;NES hardware-dependent functions by Shiru (shiru@mail.ru)
;with improvements by VEG
;Feel free to do anything you want with this code, consider it Public Domain

;nesdoug version, 2019-09
;minor change %%, added ldx #0 to functions returning char
;removed sprid from c functions to speed them up
;music and nmi changed for mmc1

;SOUND_BANK is defined at the top of crt0.s
;and needs to match the bank where the music is


	.export _music_play,_music_stop,_music_pause
	.export _sfx_play,_sample_play
	.export _pad_poll,_pad_trigger,_pad_state

	;; PPU control functions
	.export _ppu_off,_ppu_on_all,_ppu_on_bg,_ppu_on_spr,_ppu_mask,_ppu_system
	.export _ppu_wait_frame,_ppu_wait_nmi

	;; Palette control functions
	.export _pal_all,_pal_bg,_pal_spr,_pal_col,_pal_clear
	.export _pal_bright,_pal_spr_bright,_pal_bg_bright
	
	; VRAM update functions
	.export _vram_adr,_vram_put,_vram_fill,_vram_inc,_vram_unrle
	.export _set_vram_update, _vram_read,_vram_write

	.export _memcpy,_memfill,_delay,_clock
	.export _get_at_addr

	; Sprite functions
	.export _oam_clear,_oam_size,_oam_spr,_oam_meta_spr,_oam_hide
	.export _oam_set, _oam_get

	.importzp sreg
	

;------------------------
nmi:	;NMI handler
	pha
	txa
	pha
	tya
	pha

	lda <PPU_MASK_VAR	;if rendering is disabled, do not access the VRAM at all
	and #%00011000
	beq @skipAll

@doUpdate:

	lda #>OAM_BUF			;update OAM
	sta PPU_OAM_DMA

@updPalette:

	lda <PAL_UPDATE			;update palette needed?
	beq @updVRAM

	jsr _palette_update_nmi

@updVRAM:
	
	lda <NAME_UPDATE	;update VRAM needed?
	beq @doneUpdate

	jsr _vram_update_nmi

@doneUpdate:

	lda #0
	sta PPU_ADDR
	sta PPU_ADDR

	lda <PPU_CTRL_VAR
	sta PPU_CTRL

@skipAll:

	lda <PPU_MASK_VAR
	sta PPU_MASK

	inc <FRAME_CNT1
	inc <FRAME_CNT2
	lda <FRAME_CNT2
	cmp #6
	bne @skipNtsc
	lda #0
	sta <FRAME_CNT2

@skipNtsc:

	inc     <CLOCK
	bne     @doneClock
	inc     <CLOCK+1
	
@doneClock:	

	;switch the music into the prg bank first
	lda BP_BANK ;save current prg bank
	pha
	lda #SOUND_BANK
	jsr _set_prg_bank
	jsr FamiToneUpdate
	pla
	sta BP_BANK ;restore prg bank
	jsr _set_prg_bank

	pla
	tay
	pla
	tax
	pla

irq:

    rti
	
	
;------------------------	
_palette_update_nmi:

	ldx #0
	stx <PAL_UPDATE

	lda #$3f
	sta PPU_ADDR
	stx PPU_ADDR

	ldy PAL_BUF				;background color, remember it in X
	lda (PAL_BG_PTR),y
	sta PPU_DATA
	tax
	
	.repeat 3,I
	ldy PAL_BUF+1+I
	lda (PAL_BG_PTR),y
	sta PPU_DATA
	.endrepeat

	.repeat 3,J		
	stx PPU_DATA			;background color
	.repeat 3,I
	ldy PAL_BUF+5+(J*4)+I
	lda (PAL_BG_PTR),y
	sta PPU_DATA
	.endrepeat
	.endrepeat

	.repeat 4,J		
	stx PPU_DATA			;background color
	.repeat 3,I
	ldy PAL_BUF+17+(J*4)+I
	lda (PAL_SPR_PTR),y
	sta PPU_DATA
	.endrepeat
	.endrepeat	
	
	rts
	
	
;------------------------	
_vram_update_nmi:

	ldy #0
	sty <NAME_UPDATE

@updName:

	lda (NAME_DATA),y
	iny
	;cmp #$40				;is it a non-sequental write?
	;bcs @updNotSeq
	;sta PPU_ADDR
	;lda (NAME_DATA),y
	;iny
	;sta PPU_ADDR
	;lda (NAME_DATA),y
	;iny
	;sta PPU_DATA
	;jmp @updName

@updNotSeq:

	;tax
	;lda <PPU_CTRL_VAR
	;cpx #$80				;is it a horizontal or vertical sequence?
	;bcc @updHorzSeq
	;cpx #$ff				;is it end of the update?
	;beq @updDone
	
	cmp #$ff		; Simplified for speed (8bit-Dude)
	beq @updDone

@updVertSeq:

	;ora #$04
	;bne @updNameSeq			;bra

@updHorzSeq:

	;and #$fb

@updNameSeq:

	;sta PPU_CTRL

	;txa
	;and #$3f
	sta PPU_ADDR
	lda (NAME_DATA),y
	iny
	sta PPU_ADDR
	lda (NAME_DATA),y
	iny
	tax

@updNameLoop:

	lda (NAME_DATA),y
	iny
	sta PPU_DATA
	dex
	bne @updNameLoop

	;lda <PPU_CTRL_VAR
	;sta PPU_CTRL

	jmp @updName

@updDone:

	rts



;------------------------	
;void __fastcall__ pal_all(const char *data);

_pal_all:

	sta <PTR
	stx <PTR+1
	ldx #$00
	lda #$20

pal_copy:

	sta <LEN

	ldy #$00

@0:

	lda (PTR),y
	sta PAL_BUF,x
	inx
	iny
	dec <LEN
	bne @0

	inc <PAL_UPDATE

	rts



;void __fastcall__ pal_bg(const char *data);

_pal_bg:

	sta <PTR
	stx <PTR+1
	ldx #$00
	lda #$10
	bne pal_copy ;bra



;void __fastcall__ pal_spr(const char *data);

_pal_spr:

	sta <PTR
	stx <PTR+1
	ldx #$10
	txa
	bne pal_copy ;bra



;void __fastcall__ pal_col(unsigned char index,unsigned char color);

_pal_col:

	sta <PTR
	jsr popa
	and #$1f
	tax
	lda <PTR
	sta PAL_BUF,x
	inc <PAL_UPDATE
	rts



;void __fastcall__ pal_clear(void);

_pal_clear:

	lda #$0f
	ldx #0

@1:

	sta PAL_BUF,x
	inx
	cpx #$20
	bne @1
	stx <PAL_UPDATE
	rts



;void __fastcall__ pal_spr_bright(unsigned char bright);

_pal_spr_bright:

	tax
	lda palBrightTableL,x
	sta <PAL_SPR_PTR
	lda palBrightTableH,x	;MSB is never zero
	sta <PAL_SPR_PTR+1
	sta <PAL_UPDATE
	rts



;void __fastcall__ pal_bg_bright(unsigned char bright);

_pal_bg_bright:

	tax
	lda palBrightTableL,x
	sta <PAL_BG_PTR
	lda palBrightTableH,x	;MSB is never zero
	sta <PAL_BG_PTR+1
	sta <PAL_UPDATE
	rts



;void __fastcall__ pal_bright(unsigned char bright);

_pal_bright:

	jsr _pal_spr_bright
	txa
	jmp _pal_bg_bright



;------------------------	
;void __fastcall__ ppu_off(void);

_ppu_off:

	lda <PPU_MASK_VAR
	and #%11100111
	sta <PPU_MASK_VAR
	jmp _ppu_wait_nmi



;void __fastcall__ ppu_on_all(void);

_ppu_on_all:

	lda <PPU_MASK_VAR
	ora #%00011000

ppu_onoff:

	sta <PPU_MASK_VAR
	jmp _ppu_wait_nmi



;void __fastcall__ ppu_on_bg(void);

_ppu_on_bg:

	lda <PPU_MASK_VAR
	ora #%00001000
	bne ppu_onoff	;bra



;void __fastcall__ ppu_on_spr(void);

_ppu_on_spr:

	lda <PPU_MASK_VAR
	ora #%00010000
	bne ppu_onoff	;bra



;void __fastcall__ ppu_mask(unsigned char mask);

_ppu_mask:

	sta <PPU_MASK_VAR
	rts



;unsigned char __fastcall__ ppu_system(void);

_ppu_system:

	lda <NTSC_MODE
	ldx #0
	rts



;------------------------	
;void __fastcall__ oam_clear(void);

_oam_clear:

	ldx #0
	stx SPRID ; automatically sets sprid to zero
	lda #$ff
@1:
	sta OAM_BUF,x
	inx
	inx
	inx
	inx
	bne @1
	rts
	
	
	
;void __fastcall__ oam_set(unsigned char index);	
;to manually set the position
;a = sprid

_oam_set:
	and #$fc ;strip those low 2 bits, just in case
	sta SPRID
	rts
	
	
	
;unsigned char __fastcall__ oam_get(void);	
;returns the sprid

_oam_get:
	lda SPRID
	ldx #0
	rts
	


;void __fastcall__ oam_hide(unsigned char num);
;sprid removed

_oam_hide:

	tay
	lda #240
	ldx SPRID
	
@1:	
	sta OAM_BUF,x
	inx
	inx
	inx
	inx
	dey
	bne @1	

	rts



;void __fastcall__ oam_size(unsigned char size);

_oam_size:

	asl a
	asl a
	asl a
	asl a
	asl a
	and #$20
	sta <TEMP
	lda <PPU_CTRL_VAR
	and #$df
	ora <TEMP
	sta <PPU_CTRL_VAR

	rts



;void __fastcall__ oam_spr(unsigned char x,unsigned char y,unsigned char chrnum,unsigned char attr);
;sprid removed

_oam_spr:

	ldx SPRID
	;a = chrnum
	sta OAM_BUF+2,x

	ldy #0		;3 popa calls replacement
	lda (sp),y
	iny
	sta OAM_BUF+1,x
	lda (sp),y
	iny
	sta OAM_BUF+0,x
	lda (sp),y
	sta OAM_BUF+3,x

	lda <sp
	clc
	adc #3 ;4
	sta <sp
	bcc @1
	inc <sp+1

@1:

	txa
	clc
	adc #4
	sta SPRID
	rts



;void __fastcall__ oam_meta_spr(unsigned char x,unsigned char y,const unsigned char *data);
;sprid removed

_oam_meta_spr:

	sta <PTR
	stx <PTR+1

	ldy #1		;2 popa calls replacement, performed in reversed order
	lda (sp),y
	dey
	sta <SCRX
	lda (sp),y
	sta <SCRY
	
	ldx SPRID

@1:

	lda (PTR),y		;x offset
	cmp #$80
	beq @2
	iny
	clc
	adc <SCRX
	sta OAM_BUF+3,x
	lda (PTR),y		;y offset
	iny
	clc
	adc <SCRY
	sta OAM_BUF+0,x
	lda (PTR),y		;tile
	iny
	sta OAM_BUF+1,x
	lda (PTR),y		;attribute
	iny
	sta OAM_BUF+2,x
	inx
	inx
	inx
	inx
	jmp @1

@2:

	lda <sp
	adc #1 ;2			;carry is always set here, so it adds 3
	sta <sp
	bcc @3
	inc <sp+1

@3:

	stx SPRID
	rts



;void __fastcall__ ppu_wait_frame(void);

_ppu_wait_frame:

	lda <FRAME_CNT1
	
@1:

	cmp <FRAME_CNT1
	beq @1
	lda <NTSC_MODE
	beq @3

@2:

	lda <FRAME_CNT2
	cmp #5
	beq @2

@3:

	rts



;void __fastcall__ ppu_wait_nmi(void);

_ppu_wait_nmi:

	lda <FRAME_CNT1
	
@1:

	cmp <FRAME_CNT1
	beq @1
	rts



;void __fastcall__ vram_unrle(const unsigned char *data);

_vram_unrle:

	tay
	stx <RLE_HIGH
	lda #0
	sta <RLE_LOW

	lda (RLE_LOW),y
	sta <RLE_TAG
	iny
	bne @1
	inc <RLE_HIGH

@1:

	lda (RLE_LOW),y
	iny
	bne @11
	inc <RLE_HIGH

@11:

	cmp <RLE_TAG
	beq @2
	sta PPU_DATA
	sta <RLE_BYTE
	bne @1

@2:

	lda (RLE_LOW),y
	beq @4
	iny
	bne @21
	inc <RLE_HIGH

@21:

	tax
	lda <RLE_BYTE

@3:

	sta PPU_DATA
	dex
	bne @3
	beq @1

@4:

	rts

	

;void __fastcall__ vram_read(unsigned char *dst,unsigned int size);

_vram_read:

	sta <TEMP

	jsr popax
	sta <TEMP+2
	stx <TEMP+3

	lda PPU_DATA
	ldy #0
@1:
	lda PPU_DATA
	sta (TEMP+2),y
	iny
	
	cpy <TEMP
	bne @1

	rts



;void __fastcall__ vram_write(unsigned char *src,unsigned int size);

_vram_write:

	sta <TEMP

	jsr popax
	sta <TEMP+2
	stx <TEMP+3

	ldy #0
@1:
	lda (TEMP+2),y
	sta PPU_DATA
	iny

	cpy <TEMP
	bne @1

	rts



;void __fastcall__ music_play(unsigned char song);
;a = song #

_music_play:
	tax
	lda BP_BANK ;save current prg bank
	pha
	lda #SOUND_BANK
	jsr _set_prg_bank
	txa ;song number
	jsr FamiToneMusicPlay
	
	pla
	sta BP_BANK ;restore prg bank
	jmp _set_prg_bank
	;rts


;void __fastcall__ music_stop(void);

_music_stop:
	lda BP_BANK ;save current prg bank
	pha
	lda #SOUND_BANK
	jsr _set_prg_bank
	jsr FamiToneMusicStop
	
	pla
	sta BP_BANK ;restore prg bank
	jmp _set_prg_bank
	;rts



;void __fastcall__ music_pause(unsigned char pause);
;a = pause or not

_music_pause:
	tax
	lda BP_BANK ;save current prg bank
	pha
	lda #SOUND_BANK
	jsr _set_prg_bank
	txa ;song number
	jsr FamiToneMusicPause
	
	pla
	sta BP_BANK ;restore prg bank
	jmp _set_prg_bank
	;rts

	

;void __fastcall__ sfx_play(unsigned char sound,unsigned char channel);

_sfx_play:

.if(FT_SFX_ENABLE)
; a = channel
	and #$03
	tax
	lda @sfxPriority,x
	tax
	
	lda BP_BANK ;save current prg bank
	pha
	lda #SOUND_BANK
	jsr _set_prg_bank
	
	jsr popa ;a = sound
	;x = channel offset
	jsr FamiToneSfxPlay
	
	pla
	sta BP_BANK ;restore prg bank
	jmp _set_prg_bank
	;rts

@sfxPriority:

	.byte FT_SFX_CH0,FT_SFX_CH1,FT_SFX_CH2,FT_SFX_CH3
	
.else
	rts
.endif


;void __fastcall__ sample_play(unsigned char sample);
;a = sample #

_sample_play:
.if(FT_DPCM_ENABLE)
	tax
	lda BP_BANK ;save current prg bank
	pha
	lda #SOUND_BANK
	jsr _set_prg_bank
	txa ;sample number
	jsr FamiToneSamplePlay
	
	pla
	sta BP_BANK ;restore prg bank
	jmp _set_prg_bank
	;rts
.else
	rts
.endif


;unsigned char __fastcall__ pad_poll(unsigned char pad);

_pad_poll:

	tay
	ldx #3

@padPollPort:

	lda #1
	sta CTRL_PORT1
	sta <PAD_BUF-1,x
	lda #0
	sta CTRL_PORT1
	lda #8
	sta <TEMP

@padPollLoop:

	lda CTRL_PORT1,y
	lsr a
	rol <PAD_BUF-1,x
	bcc @padPollLoop

	dex
	bne @padPollPort

	lda <PAD_BUF
	cmp <PAD_BUF+1
	beq @done
	cmp <PAD_BUF+2
	beq @done
	lda <PAD_BUF+1

@done:

	sta <PAD_STATE,y
	tax
	eor <PAD_STATEP,y
	and <PAD_STATE ,y
	sta <PAD_STATET,y
	txa
	sta <PAD_STATEP,y
	
	ldx #0
	rts



;unsigned char __fastcall__ pad_trigger(unsigned char pad);

_pad_trigger:

	pha
	jsr _pad_poll
	pla
	tax
	lda <PAD_STATET,x
	ldx #0
	rts



;unsigned char __fastcall__ pad_state(unsigned char pad);

_pad_state:

	tax
	lda <PAD_STATE,x
	ldx #0
	rts



;void __fastcall__ set_vram_update(unsigned char *buf);

_set_vram_update:

	sta <NAME_DATA+0
	stx <NAME_DATA+1
	ora <NAME_DATA+1
	sta <NAME_UPDATE

	rts
	
	
	
;void __fastcall__ vram_adr(unsigned int adr);

_vram_adr:

	stx PPU_ADDR
	sta PPU_ADDR

	rts



;void __fastcall__ vram_put(unsigned char n);

_vram_put:

	sta PPU_DATA

	rts



;void __fastcall__ vram_fill(unsigned char n,unsigned int len);

_vram_fill:

	sta <LEN
	stx <LEN+1
	jsr popa
	ldx <LEN+1
	beq @2
	ldx #0

@1:

	sta PPU_DATA
	dex
	bne @1
	dec <LEN+1
	bne @1

@2:

	ldx <LEN
	beq @4

@3:

	sta PPU_DATA
	dex
	bne @3

@4:

	rts



;void __fastcall__ vram_inc(unsigned char n);

_vram_inc:

	ora #0
	beq @1
	lda #$04

@1:

	sta <TEMP
	lda <PPU_CTRL_VAR
	and #$fb
	ora <TEMP
	sta <PPU_CTRL_VAR
	sta PPU_CTRL

	rts



;void __fastcall__ memcpy(void *dst,void *src,unsigned int len);

_memcpy:

	sta <LEN
	stx <LEN+1
	jsr popax
	sta <SRC
	stx <SRC+1
	jsr popax
	sta <DST
	stx <DST+1

	ldx #0

@1:

	lda <LEN+1
	beq @2
	jsr @3
	dec <LEN+1
	inc <SRC+1
	inc <DST+1
	jmp @1

@2:

	ldx <LEN
	beq @5

@3:

	ldy #0

@4:

	lda (SRC),y
	sta (DST),y
	iny
	dex
	bne @4

@5:

	rts



;void __fastcall__ memfill(void *dst,unsigned char value,unsigned int len);

_memfill:

	sta <LEN
	stx <LEN+1
	jsr popa
	sta <TEMP
	jsr popax
	sta <DST
	stx <DST+1

	ldx #0

@1:

	lda <LEN+1
	beq @2
	jsr @3
	dec <LEN+1
	inc <DST+1
	jmp @1

@2:

	ldx <LEN
	beq @5

@3:

	ldy #0
	lda <TEMP

@4:

	sta (DST),y
	iny
	dex
	bne @4

@5:

	rts

;clock_t __fastcall__ clock(unsigned char frames);

_clock:

	ldy     #0              ; High word is always zero
	sty     sreg+1
	sty     sreg
	ldx     CLOCK+1
	lda     CLOCK
	
	rts


;void __fastcall__ delay(unsigned char frames);

_delay:

	tax

@1:

	jsr _ppu_wait_nmi
	dex
	bne @1

	rts
	
	
;int __fastcall__ get_at_addr(char nt, char x, char y);
_get_at_addr:
	and #$e0
	sta TEMP
	
	jsr popa
	and #$e0
	lsr a
	lsr a
	lsr a
	ora TEMP
	lsr a
	lsr a
	ora #$c0
	sta TEMP
	
	jsr popa
	and #3
	asl a
	asl a
	ora #$23
	tax
	lda TEMP
	rts	



palBrightTableL:

	.byte <palBrightTable0,<palBrightTable1,<palBrightTable2
	.byte <palBrightTable3,<palBrightTable4,<palBrightTable5
	.byte <palBrightTable6,<palBrightTable7,<palBrightTable8

palBrightTableH:

	.byte >palBrightTable0,>palBrightTable1,>palBrightTable2
	.byte >palBrightTable3,>palBrightTable4,>palBrightTable5
	.byte >palBrightTable6,>palBrightTable7,>palBrightTable8

palBrightTable0:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f	;black
palBrightTable1:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable2:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable3:
	.byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
palBrightTable4:
	.byte $00,$01,$02,$03,$04,$05,$06,$07,$08,$09,$0a,$0b,$0c,$0f,$0f,$0f	;normal colors
palBrightTable5:
	.byte $10,$11,$12,$13,$14,$15,$16,$17,$18,$19,$1a,$1b,$1c,$00,$00,$00
palBrightTable6:
	.byte $10,$21,$22,$23,$24,$25,$26,$27,$28,$29,$2a,$2b,$2c,$10,$10,$10	;$10 because $20 is the same as $30
palBrightTable7:
	.byte $30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3a,$3b,$3c,$20,$20,$20
palBrightTable8:
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30	;white
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
	.byte $30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
