
#include "unity.h"

extern const char keyNext, pressKeyMsg[];

int DemoGfx(void) 
{
	unsigned char i, color, row, palette;
	unsigned char textRows, blockW, blockH;
	
	// Prepare bitmap
	InitBitmap();
	ClearBitmap();
	ShowBitmap();
	
	// Draw palette (Method #1: set pixel coordinates directly using global variables pixelX & pixelY)
#if defined __ORIC__	
	palette = 19; blockW = 6;	// Oric Hires (enforce 6 pixel groups)
#elif defined __SHR__	
	palette = 6;  blockW = 21;	// Apple Single Hires (enforce 7 pixel groups)
#else	
	palette = BMP_PALETTE; 
	blockW = BMP_COLS/BMP_PALETTE;	
#endif
	blockH = (8*BMP_ROWS)/200;		// Display equivalent of 8 lines (rescaled on APPLE/ATMOS/LYNX)
	for (color=0; color<palette; color++) {
		for (row=0; row<blockH; row++) {
			for (i=0; i<blockW; i++) {
				pixelY = row;
				pixelX = (color)*blockW + i;
			#if defined __ORIC__
				SetPixel(color+1);	// Don't show black (only space for 19 colors)
			#else	
				SetPixel(color);
			#endif
			}
		}
	}
	
	// Print text (global variables inkColor & paperColor are assigned directly)
#if defined __ORIC__
	textRows = 19;
#elif defined __LYNX__
	textRows = 12;
#elif defined __SHR__
	textRows = 6;
#else 
	textRows = 16;
#endif		
	paperColor = BLACK;
	for (i=1; i<textRows; i++) {
		inkColor = i;
	#if defined __ORIC__
		SetAttributes(-1, i+1, inkColor);
	#endif
		PrintStr(15,i+1,"8BIT-UNITY");
	}
	
	for (i=0; i<5; i++) { 
		PrintLogo(2, 3+i*2, i);
		PrintLogo(TXT_COLS-3, 3+i*2, i);
	}
	
	
	// Draw Line (Method #2: locate pixel against a 320 x 200 screen)
	for (i=0; i<20; i++) {
		LocatePixel(140+i*2, 170);
		SetPixel(WHITE);
	}

	// Wait until 'SPACE' is pressed
	inkColor = BLACK; paperColor = WHITE; 
	PrintStr(9, TXT_ROWS-2, pressKeyMsg);
	while (!kbhit () || cgetc () != keyNext) {	
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}
	
    // Done
	HideBitmap();
    return EXIT_SUCCESS;	
}
