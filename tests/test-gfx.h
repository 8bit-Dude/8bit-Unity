
#include "unity.h"

int TestGFX(void) 
{
	unsigned char i,j;

	// Reset screen
	clrscr();
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);

	// Prepare bitmap
	InitBitmap();
	ClearBitmap();
	EnterBitmapMode();
	
	// Draw palette
	for (i=0; i<16; i++) {
		for (j=0; j<10; j++) {
			SetColor(i*20+2*j,0,i);
			SetColor(i*20+2*j,1,i);
			SetColor(i*20+2*j,2,i);
			SetColor(i*20+2*j,3,i);
			SetColor(i*20+2*j,4,i);
			SetColor(i*20+2*j,5,i);
		}
	}
	
	// Draw text
	colorBG = BLACK;
	for (i=1; i<16; i++) {
		colorFG = i;
		PrintStr(10,i,"8BIT-UNITY IS EASY!");
	}	
	colorFG = WHITE;
	PrintStr(7,16,"PRESS SPACE FOR NEXT TEST");

	// Wait until keyboard is pressed
	cgetc();
	ExitBitmapMode();
	
    // Done
    return EXIT_SUCCESS;	
}