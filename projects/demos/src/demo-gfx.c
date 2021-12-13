
#include "unity.h"

#ifdef __NES__
  #define LINE_SPACING  2
  #define TEXT_COL	   11
  #define TEXT_ROW	    6
#else
  #define LINE_SPACING  1
  #define TEXT_COL	   15
  #define TEXT_ROW	    2
#endif

#if defined __ORIC__
  #define NUM_ROWS 	19
#elif defined __LYNX__
  #define NUM_ROWS 	12
#elif defined __NES__
  #define NUM_ROWS   4
#elif defined __SHR__
  #define NUM_ROWS   6
#else 
  #define NUM_ROWS  16
#endif		
	
extern const char nextKey, nextMsg[];
extern unsigned char nextCol;

int DemoGfx(void) 
{
	unsigned char i, color, row, palette;
	unsigned char blockW, blockH;
	
	// Prepare bitmap
	InitBitmap();
	ClearBitmap();
	ShowBitmap();
	
	// Draw palette (Method #1: set pixel coordinates directly using global variables pixelX & pixelY)
#ifndef __NES__	
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
#endif
	
	// Print text (global variables inkColor & paperColor are assigned directly)
	paperColor = BLACK;
	txtX = TEXT_COL; txtY = TEXT_ROW;
	for (i=0; i<NUM_ROWS; i++) {
		inkColor = i;
		PrintStr("8BIT-UNITY"); 
	#if defined __ORIC__
		txtX--; SetAttributes(inkColor); txtX++;
	#endif
		txtY += LINE_SPACING;
	}
	
	// Show Platform logos
	txtY = 3;
	for (i=0; i<6; i++) { 
		txtX = 2;		   PrintLogo(i);
		txtX = TXT_COLS-3; PrintLogo(i);
		txtY += 2;
	}
	
	// Draw Line (Method #2: locate pixel against a 320 x 200 screen)
#ifndef __NES__		
	for (i=0; i<20; i++) {
		LocatePixel(140+i*2, 170);
		SetPixel(WHITE);
	}
#endif		

	// Show "next" Message
	inkColor = BLACK; paperColor = WHITE; 
	txtX = nextCol; txtY = TXT_ROWS-2;
	PrintStr(nextMsg);
	
	// Wait until 'SPACE' is pressed
	while (!kbhit () || cgetc () != nextKey) {	
	#if defined(__LYNX__) || defined(__NES__)
		UpdateDisplay(); // Manually refresh Display
	#endif		
	}
	
    // Done
	HideBitmap();
    return EXIT_SUCCESS;	
}
