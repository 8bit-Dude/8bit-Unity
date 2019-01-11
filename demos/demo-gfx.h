
#include "unity.h"

int DemoGFX(void) 
{
	unsigned char i,j,k;

	// Prepare bitmap
	InitBitmap();
	ClearBitmap();
	EnterBitmapMode();
	
	// Draw palette
	for (i=0; i<16; i++) {
		for (j=0; j<10; j++) {
			for (k=0; k<6; k++) {
				SetColor(i*20+j*2, k, i);
			}
		}
	}
	
	// Draw text
	colorBG = BLACK;
	for (i=1; i<16; i++) {
		colorFG = i;
		PrintStr(15,i,"8BIT-UNITY");
	}	
	colorFG = WHITE;
	PrintStr(8,16,"PRESS SPACE FOR NEXT TEST");

	// Wait until keyboard is pressed
	cgetc();
	ExitBitmapMode();
	
    // Done
    return EXIT_SUCCESS;	
}