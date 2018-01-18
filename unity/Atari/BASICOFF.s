
        .include        "atari.inc"
        .export         __AUTOSTART__: absolute = 1
        .export         __EXEHDR__: absolute = 1

.segment        "EXEHDR"
        .word   $FFFF

.segment        "MAINHDR"
        .word   $d301
        .word   $d301

.data
	.byte	$ff


