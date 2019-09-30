


		.macro CHAN_STOP
		.byte 0
		.endmacro

		;* 
        ;* To be Changed
		.macro PLAY Note, Delay
		.byte Note
		IF Note > 255
		.byte 0
ps2     .set Delay-256
		DELAY ps2
		ELSE
		.byte Delay
		ENDIF
		.endmacro

		.macro DO Do
		.byte $80,Do
		.endmacro
		.macro LOOP
		.byte $81
		.endmacro

		.macro DELAY Delay
ps    .set Delay
		.repeat 50
		.IF ps > 255
		  .byte $82,0
ps    .set ps - 256
		.ELSE
		  .IF ps > 0
			.byte $82,ps
ps     .set 0
		  .ENDIF
		.ENDIF
		.endrepeat
		.endmacro

		.macro STOP
		.byte $83
		.endmacro

		.macro SET_INSTR p1, p2, p3
		.byte $84,0,0,p1,p2,p3
		.endmacro

		;* INST WaveEnv,VolEnv
		.macro INSTR2 p1, p2, p3, p4, p5
		.byte $84, p1, p2, p3, p4, p5
		.endmacro

		;* PLAY2 RELOAD, PRESCALE, DELAY
        ;* To be Changed
		.macro PLAY2 p1,p2,p3
		.byte $85
		.byte p1     ; reload
		.byte p2     ; prescale
		.IF p3 > 255
		.byte 0
ps2     .set p3-256
		DELAY ps2
		.ELSE
		.byte p3
		.ENDIF
		.endmacro


		;* Jump to Pattern
		.macro PATTERN Pat
		.byte $86
		.word Pat
		.endmacro

		;* Return from Pattern
		.macro RETURN
		.byte $87
		.endmacro

		.macro DEF_VOLENV Nr, Ptr
		.byte $88,Nr
		.word Ptr
		.endmacro

		.macro SET_VOLENV Nr
		.byte $89,Nr
		.endmacro

		.macro DEF_FRQENV Nr, Ptr
		.byte $8a,Nr
		.word Ptr
		.endmacro

		.macro SET_FRQENV Nr
		.byte $8b,Nr
		.endmacro

		.macro DEF_WAVEENV Nr, Ptr
		.byte $8c,Nr
		.word Ptr
		.endmacro

		.macro SET_WAVEENV Nr
		.byte $8d,Nr
		.endmacro

		;* Set Stereo / $FD50
		;* for all channels
		.macro SET_STEREO Ste
		.byte $8e,Ste
		.endmacro

		;* Set Attenuation On/Off / $FD44
		;* for all channels
		.macro SET_ATT_ON Att
		.byte $8f,Att
		.endmacro

		;* Set Attenuation per Channel / $FD40-43
		.macro SET_CH_ATT Att
		.byte $90,Att
		.endmacro

		;* SETPLAYERFREQ RELOAD, PRESCALE
		;* work globally and direct!
		.macro SETPLAYERFREQ p1,p2
		.byte $91,p1,p2
		.endmacro

		;* RETURNALL/BRK
		;* work globally and direct!
		.macro RETURNALL
		.byte $92
		.endmacro

