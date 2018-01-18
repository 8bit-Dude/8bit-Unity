
.export _DisableRom, _EnableRom

_DisableRom:
	lda	1
	sta	old_port1
	and	#%11111100
	sei
	sta	1
	rts

_EnableRom:
	lda	old_port1
	sta	1
	cli
	rts

.bss
old_port1:
	.res	1
