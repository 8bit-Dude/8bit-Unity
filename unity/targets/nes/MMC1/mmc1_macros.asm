MMC1_CTRL	=$8000
MMC1_CHR0	=$a000
MMC1_CHR1	=$c000
MMC1_PRG	=$e000


; MMC1 reset code removed, see crt0.s


; macro to write to an mmc1 register, which goes one bit at a time, 5 bits wide.
.macro mmc1_register_write addr
	.repeat 4
		sta addr
		lsr
	.endrepeat
	sta addr
.endmacro