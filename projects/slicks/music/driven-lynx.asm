
;;; source file for CC65 "remake" compiler
	.include "soundbs.mac"
	.global musicptr
	.rodata
musicptr:
	.word 0, 0, CHANNEL_0, CHANNEL_1

PATTERN_LENGTH   .set  2048

CHANNEL_0:
  DO 0
	PATTERN ENVIROS
	PATTERN PATTERN_06
	PATTERN PATTERN_00
	PATTERN PATTERN_00
	PATTERN PATTERN_05
	PATTERN PATTERN_00
	PATTERN PATTERN_00
  LOOP
CHAN_STOP

CHANNEL_1:
  DO 0
	PATTERN ENVIROS
	PATTERN PATTERN_01
	PATTERN PATTERN_01
	PATTERN PATTERN_01
	PATTERN PATTERN_01
	PATTERN PATTERN_01
	PATTERN PATTERN_01
  LOOP
CHAN_STOP

ENVIROS:
	SETPLAYERFREQ $9D,129
	DEF_VOLENV 4,TREMOLO_4
	DEF_VOLENV 8,TREMOLO_8
	DEF_WAVEENV 25,WAVEENV_25
	DEF_VOLENV 26,TREMOLO_26
	DELAY 3
RETURN

TREMOLO_4:
.byte $02,$02,$0B,$00,$01,$F6
TREMOLO_8:
.byte $02,$02,$05,$00,$01,$FB
WAVEENV_25:
.byte 2,2,20,$48,$10,$80,100,$bd,$0,$d0
TREMOLO_26:
.byte $02,$02,$29,$00,$01,$FF

PATTERN_00:
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 219,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 164,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 138,$59,32
	PLAY2 219,$58,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 246,$58,32
	PLAY2 138,$59,64
	PLAY2 246,$58,64
	PLAY2 184,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 164,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 138,$59,32
	PLAY2 219,$58,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 246,$58,32
	PLAY2 138,$59,64
	PLAY2 164,$59,64
	PLAY2 184,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 164,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 138,$59,32
	PLAY2 219,$58,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 246,$58,32
	PLAY2 138,$59,64
	PLAY2 246,$58,64
	PLAY2 184,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 195,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 164,$59,64
	PLAY2 138,$59,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 164,$59,32
	PLAY2 138,$59,32
	PLAY2 246,$58,64
RETURN


PATTERN_01:
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,32
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 138,$59,32
	PLAY2 164,$59,32
	PLAY2 246,$58,32
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 219,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 219,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,32
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 195,$59,32
	PLAY2 219,$59,32
	PLAY2 164,$59,32
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 219,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 184,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 184,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,32
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 246,$58,32
	PLAY2 138,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 184,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,32
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,32
	PLAY2 195,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,32
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,32
	PLAY2 195,$59,64
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,64
RETURN


PATTERN_05:
	DELAY 32
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 164,$59,32
	PLAY2 138,$59,64
	PLAY2 246,$58,64
	PLAY2 138,$59,64
	PLAY2 232,$58,64
	PLAY2 138,$59,64
	PLAY2 164,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 184,$59,64
	PLAY2 164,$59,64
	PLAY2 138,$59,64
	PLAY2 184,$59,64
	PLAY2 164,$59,128
	PLAY2 184,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 219,$58,32
	PLAY2 246,$58,128
	PLAY2 246,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 138,$59,64
	PLAY2 219,$58,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 246,$58,128
	PLAY2 138,$59,64
	PLAY2 164,$59,64
	PLAY2 246,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $48,$10,$80,28,127
	SET_VOLENV 8
	SET_WAVEENV 25
	PLAY2 138,$59,64
	PLAY2 164,$59,96
	PLAY2 246,$58,96
RETURN


PATTERN_06:
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 164,$59,32
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 138,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 219,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 219,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 219,$59,32
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 219,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 184,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 146,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 138,$59,32
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,64
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 146,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	PLAY2 3,$58,32
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 246,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	PLAY2 3,$58,32
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 195,$59,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	PLAY2 3,$58,32
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 246,$58,64
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	PLAY2 3,$58,32
	INSTR2 $7d,$00,$e6,17,127
	SET_VOLENV 26
	PLAY2 195,$59,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
	INSTR2 $24,$40,$b5,127,127
	SET_VOLENV 4
	PLAY2 133,$5d,32
	INSTR2 $60,$20,$7b,56,127
	SET_VOLENV 8
	PLAY2 3,$58,32
RETURN

