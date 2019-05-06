
#include "unity.h"

int DemoGFX(void) 
{
	unsigned char i, color, row, blockW, blockH;

	// Prepare bitmap
	InitBitmap();
	ClearBitmap();
	EnterBitmapMode();
	
	// Draw palette (Method #1: set pixel coordinates directly using global variables bmpX & bmpY)
	blockW = BMPWIDTH/(BMPCOLORS-1);	// Cover width with available palette
	blockH = (8*BMPHEIGHT)/200;		// Display equivalent of 8 lines (rescaling on APPLE/ATMOS)
	for (color=1; color<BMPCOLORS; color++) {
		for (row=0; row<blockH; row++) {
			for (i=0; i<blockW; i++) {
				bmpY = row;
				bmpX = (color-1)*blockW + i;
				SetPixel(color);
			}
		}
	}
	
	// Print text (global variables inkColor & paperColor are assigned directly)
	paperColor = BLACK;
	for (i=1; i<BMPCOLORS; i++) {
		inkColor = i;
		PrintStr(15,i+1,"8BIT-UNITY");
	}	
	
	// Draw Line (Method #2: locate pixel against a 320 x 200 screen)
	for (i=0; i<20; i++) {
		LocatePixel(140+i*2, (BMPCOLORS+2)*8);
		SetPixel(WHITE);
	}
	
	// Wait until keyboard is pressed
	inkColor = BLACK; paperColor = WHITE; 
	PrintStr(8,BMPCOLORS+3,"PRESS SPACE FOR NEXT TEST");
	cgetc();
	
    // Done
	ExitBitmapMode();
    return EXIT_SUCCESS;	
}