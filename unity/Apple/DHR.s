
	.export		_DHRSetColor

MBOFFSET DB 255,0,0,0,255,1,1
ABOFFSET DB 0,0,255,1,1,1,255
MAINGR DB %00000000,%00000000,%00001100,%01000000
 DB %00000000,%00000011,%00110000
AUXGR DB %00000110,%01100000,%00000000,%000000001
 DB %00011000,%00000000,%00000000
MAINAND DB %01111111,%01111110,%01100001,%00011111
 DB %01111111,%01111000,%00000111
AUXAND DB %01110000,%00001111,%01111111,%01111100
 DB %01000011,%00111111,%01111111

 .segment	"CODE"

.proc	_DHRSetColor: near

	LDX #00
DPLOT:
	LDY MBOFFSET,X ;Find what byte if any in MAIN we are working in
	BMI AUX ;If pixel has not bits in MAIN memory - go to aux routine
	STA PAGE1 ;Map $2000 to MAIN memory
	LDA $2000,Y ;Load screen data
	AND MAINAND,X ;Erase pixel bits
	ORA MAINGR,X ;Draw green pixel
	STA $2000,Y ;Write back to screen
AUX: 
	LDY ABOFFSET,X ;Find what byte if any in MAIN we are working in
	BMI END ;If no part of the pixel is in AUX - end the program
	STA PAGE2 ;Map $2000 to AUX memory
	LDA $2000,Y ;Load screen data
	AND AUXAND,X ;Erase pixel bits
	ORA AUXGR,X ;Draw green pixel
	STA $2000,Y ;Write back to screen
END: 
	RTS
