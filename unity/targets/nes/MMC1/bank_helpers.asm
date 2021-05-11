

.define NO_CHR_BANK 255

.segment "ZEROPAGE"
	BP_BANK: .res 1
    nmiChrTileBank: .res 1
    .exportzp nmiChrTileBank, BP_BANK

.segment "CODE"

.export _set_prg_bank, _get_prg_bank, _set_chr_bank_0, _set_chr_bank_1
.export _set_nmi_chr_tile_bank, _unset_nmi_chr_tile_bank
.export _set_mirroring, _set_mmc1_ctrl

; sets the bank at $8000-bfff
_set_prg_bank:
    sta BP_BANK
    mmc1_register_write MMC1_PRG
    rts

; returns the current bank at $8000-bfff	
_get_prg_bank:
    lda BP_BANK
	ldx #0
    rts

; sets the first CHR bank
_set_chr_bank_0:
    mmc1_register_write MMC1_CHR0
    rts

; sets the second CHR bank	
_set_chr_bank_1:
    mmc1_register_write MMC1_CHR1
    rts

; for split screens with different CHR bank at top
_set_nmi_chr_tile_bank: 
    sta nmiChrTileBank
    rts

; for split screens with different CHR bank at top... disable it
_unset_nmi_chr_tile_bank:
    lda #NO_CHR_BANK
    sta nmiChrTileBank
    rts

_set_mirroring:
    ; Limit this to mirroring bits, so we can add our bytes safely.
    and #%00000011
    ; Now, set this to have 4k chr banking, and not mess up which prg bank is which.
    ora #%00011100
Ctrl_common:
    mmc1_register_write MMC1_CTRL
    rts
	
; write all 5 bits of the $8000 control register
; in case you want to change the MMC1 settings besides mirroring (not recommended)
_set_mmc1_ctrl:
	and #$1f ;remove upper 3 bits
	jmp Ctrl_common
	
	
	