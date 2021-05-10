
; startup code for cc65 and neslib
; based on code by Groepaz/Hitmen <groepaz@gmx.net>, Ullrich von Bassewitz <uz@cc65.org>

;v050517

FT_DPCM_OFF = __DMC_START__			;set in the linker CFG file via MEMORY/DMC section
									;'start' there should be $c000..$ffc0, in 64-byte steps
FT_SFX_STREAMS = 4			        ;number of sound effects played at once, 1..4

.define FT_MUSIC_ENABLE   1			;undefine to exclude all music code
.define FT_DPCM_ENABLE    0			;undefine to exclude all DMC code
.define FT_SFX_ENABLE     0			;undefine to exclude all sound effects code

	.export PTR
    .export _exit,__STARTUP__:absolute=1
	.import initlib,push0,popa,popax,_main,zerobss,copydata

; Linker generated symbols
	.import __RAM_START__   ,__RAM_SIZE__
	.import __ROM0_START__  ,__ROM0_SIZE__
	.import __STARTUP_LOAD__,__STARTUP_RUN__,__STARTUP_SIZE__
	.import	__CODE_LOAD__   ,__CODE_RUN__   ,__CODE_SIZE__
	.import	__RODATA_LOAD__ ,__RODATA_RUN__ ,__RODATA_SIZE__
	.import __DMC_START__
	.import NES_MAPPER,NES_PRG_BANKS,NES_CHR_BANKS,NES_MIRRORING
    .include "zeropage.inc"
	
; Assets

	.import _music_data


FT_BASE_ADR		=$0100	;page in RAM, should be $xx00

.define FT_THREAD       1	;undefine if you call sound effects in the same thread as sound update
.define FT_PAL_SUPPORT	1   ;undefine to exclude PAL support
.define FT_NTSC_SUPPORT	1   ;undefine to exclude NTSC support


PPU_CTRL			=$2000
PPU_MASK			=$2001
PPU_STATUS			=$2002
PPU_OAM_ADDR		=$2003
PPU_OAM_DATA		=$2004
PPU_SCROLL			=$2005
PPU_ADDR			=$2006
PPU_DATA			=$2007
PPU_OAM_DMA			=$4014
DMC_FREQ			=$4010
CTRL_PORT1			=$4016
CTRL_PORT2			=$4017

OAM_BUF				=$0200
PAL_BUF				=$01c0



.segment "ZEROPAGE"

NTSC_MODE: 			.res 1
FRAME_CNT1: 		.res 1
FRAME_CNT2: 		.res 1
NAME_UPD_ADR: 		.res 2
NAME_UPD_ENABLE: 	.res 1
PAL_UPDATE: 		.res 1
PAL_BG_PTR: 		.res 2
PAL_SPR_PTR: 		.res 2
SCROLL_X: 			.res 1
SCROLL_Y: 			.res 1
SCROLL_X1: 			.res 1
SCROLL_Y1: 			.res 1
PAD_STATE: 			.res 2		;one byte per controller
PAD_STATEP: 		.res 2
PAD_STATET: 		.res 2
PPU_CTRL_VAR: 		.res 1
PPU_CTRL_VAR1: 		.res 1
PPU_MASK_VAR: 		.res 1
FT_TEMP: 			.res 3

TEMP: 				.res 11

PAD_BUF				= TEMP+1

PTR					= TEMP	    ;word
LEN					= TEMP+2	;word
NEXTSPR				= TEMP+4
SCRX				= TEMP+5
SCRY				= TEMP+6
SRC					= TEMP+7	;word
DST					= TEMP+9	;word

RLE_LOW				= TEMP
RLE_HIGH			= TEMP+1
RLE_TAG				= TEMP+2
RLE_BYTE			= TEMP+3



.segment "HEADER"

    .byte $4e,$45,$53,$1a
	.byte <NES_PRG_BANKS
	.byte <NES_CHR_BANKS
	.byte <NES_MIRRORING|(<NES_MAPPER<<4)
	.byte <NES_MAPPER&$f0
	.res 8,0



.segment "STARTUP"

start:
_exit:

    sei
    ldx #$ff
    txs
    inx
    stx PPU_MASK
    stx DMC_FREQ
    stx PPU_CTRL		;no NMI

initPPU:

    bit PPU_STATUS
@1:
    bit PPU_STATUS
    bpl @1
@2:
    bit PPU_STATUS
    bpl @2

clearPalette:

	lda #$3f
	sta PPU_ADDR
	stx PPU_ADDR
	lda #$0f
	ldx #$20
@1:
	sta PPU_DATA
	dex
	bne @1

clearVRAM:

	txa
	ldy #$20
	sty PPU_ADDR
	sta PPU_ADDR
	ldy #$10
@1:
	sta PPU_DATA
	inx
	bne @1
	dey
	bne @1

clearRAM:

    txa
@1:
    sta $000,x
    sta $100,x
    sta $200,x
    sta $300,x
    sta $400,x
    sta $500,x
    sta $600,x
    sta $700,x
    inx
    bne @1

	lda #4
	jsr _pal_bright
	;jsr _pal_clear
	jsr _oam_clear

    jsr	zerobss
	jsr	copydata

    lda #<(__RAM_START__+__RAM_SIZE__)
    sta	sp
    lda	#>(__RAM_START__+__RAM_SIZE__)
    sta	sp+1            ; Set argument stack ptr

	jsr	initlib

	lda #%10001000		;sprites use page 2
	sta <PPU_CTRL_VAR
	sta PPU_CTRL		;enable NMI
	lda #%00000110
	sta <PPU_MASK_VAR

waitSync3:
	lda <FRAME_CNT1
@1:
	cmp <FRAME_CNT1
	beq @1

detectNTSC:
	ldx #52				;blargg's code
	ldy #24
@1:
	dex
	bne @1
	dey
	bne @1

	lda PPU_STATUS
	and #$80
	sta <NTSC_MODE

	jsr _ppu_off

.if(FT_MUSIC_ENABLE)
	ldx #<_music_data
	ldy #>_music_data
	lda <NTSC_MODE
	jsr FamiToneInit
.endif

.if(FT_SFX_ENABLE)
	ldx #<sounds_data
	ldy #>sounds_data
	jsr FamiToneSfxInit
.endif

	lda #0
	sta PPU_SCROLL
	sta PPU_SCROLL			
	sta PPU_OAM_ADDR

	jmp _main			;no parameters

	.include "neslib.s"

.segment "RODATA"

.if(FT_SFX_ENABLE)
sounds_data:
	.include "sounds.s"
.endif

.segment "SAMPLES"

.if(FT_DPCM_ENABLE)
	.incbin "music.dmc"
.endif

.segment "VECTORS"

    .word nmi	;$fffa vblank nmi
    .word start	;$fffc reset
   	.word irq	;$fffe irq / brk
