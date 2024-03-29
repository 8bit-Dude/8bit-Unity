
#include "definitions.h"

#if defined(__NES__)
 #pragma rodata-name("BANK0")
 #pragma code-name("BANK0")
#endif

// See Unity
extern unsigned char* fileNames[];

char imageSel = 127, imageShowing = 0;
callback *iPrev, *iNext;

void ImageDisplay()
{
	// Clear screen and callbacks first!
	ClearScreen();
	
	// Disable music and display bitmap
	PauseTrack();
	LoadBitmap(fileNames[imageSel]);
	UnpauseTrack();
	DrawTaskBar();
		
	// Display file name
	paperColor = WHITE; inkColor = BLACK; 
	txtX = 14; txtY = TXT_ROWS-1;	
	PrintStr(fileNames[imageSel]);		
	
	// Add L/R Controllers
	paperColor = BLACK; inkColor = WHITE;
	iPrev = Button(10, TXT_ROWS-1, 3, 1, " ( ");	
	iNext = Button(27, TXT_ROWS-1, 3, 1, " ) ");		
}

void ImageScreen()
{		
	if (imageSel == 127)
		SelectFile(1, imageExt, &imageSel);
	ImageDisplay();
}

void ImageCallback(callback* call)
{	
	// Select next file in chosen direction
	char dir = 1;
	if (call == iPrev)
		dir = -1;
	SelectFile(dir, imageExt, &imageSel);
	ImageDisplay();
}
