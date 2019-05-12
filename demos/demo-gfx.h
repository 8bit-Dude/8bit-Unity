
#include "unity.h"

int DemoGFX(void) 
{
	unsigned char i, color, row, blockW, blockH;

	// Prepare bitmap
	InitBitmap();
	ClearBitmap();
	EnterBitmapMode();
	
	// Draw palette (Method #1: set pixel coordinates directly using global variables pixelX & pixelY)
	blockW = BMP_COLS/(BMP_PALETTE-1);	// Cover width with available palette
	blockH = (8*BMP_ROWS)/200;		// Display equivalent of 8 lines (rescaling on APPLE/ATMOS)
	for (color=1; color<BMP_PALETTE; color++) {
		for (row=0; row<blockH; row++) {
			for (i=0; i<blockW; i++) {
				pixelY = row;
				pixelX = (color-1)*blockW + i;
				SetPixel(color);
			}
		}
	}
	
	// Print text (global variables inkColor & paperColor are assigned directly)
	paperColor = BLACK;
	for (i=1; i<BMP_PALETTE; i++) {
		inkColor = i;
#if defined __ATMOS__
		SetInk(14, i+1);
#endif
		PrintStr(15,i+1,"8BIT-UNITY");
	}	
	
	// Draw Line (Method #2: locate pixel against a 320 x 200 screen)
	for (i=0; i<20; i++) {
		LocatePixel(140+i*2, (BMP_PALETTE+2)*8);
		SetPixel(WHITE);
	}
	
	// Wait until keyboard is pressed
	inkColor = BLACK; paperColor = WHITE; 
	PrintStr(8,BMP_PALETTE+3,"PRESS SPACE FOR NEXT TEST");
	cgetc();
	
    // Done
	ExitBitmapMode();
    return EXIT_SUCCESS;	
}