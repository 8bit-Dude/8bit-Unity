
#include "unity.h"

extern const char keyNext;

int DemoBitmap(void) 
{
	unsigned char i, r;
	unsigned char* chunks[3];
#if defined __APPLE2__
	unsigned char coords[3][4] = { {  7, 5, 28, 39 }, {  56, 5, 28, 39 }, {105, 5, 28, 39} };
#elif defined __ATARI__
	unsigned char coords[3][4] = { {  8, 6, 36, 40 }, {  60, 6, 36, 40 }, {112, 6, 36, 40} };
#elif defined __C64__
	unsigned char coords[3][4] = { {  8, 0, 36, 48 }, {  60, 0, 36, 48 }, {112, 0, 36, 48} };
#elif defined __LYNX__
	unsigned char coords[3][4] = { { 12, 3, 30, 20 }, {  64, 3, 30, 20 }, {116, 3, 30, 20} };
#elif defined __ORIC__
	unsigned char coords[3][4] = { { 18, 4, 48, 42 }, {  96, 4, 48, 42 }, {174, 4, 48, 42} };
#endif
	clock_t time = clock();
	
	// Prepare bitmap
	InitBitmap();
	LoadBitmap("banner.img");
	EnterBitmapMode();
	
	// Grab graphic chunks
	for (i=0; i<3; i++) 
		GetChunk(&chunks[i], coords[i][0], coords[i][1], coords[i][2], coords[i][3]);
	
	// Wait until 'SPACE' is pressed
	while (!kbhit () || cgetc () != keyNext) {	
		if (clock() > time+4) {
			// Rotate chunks
			time = clock();
			for (i=0; i<3; i++) 
				SetChunk(chunks[(i+r)%3], coords[i][0], coords[i][1]);
			r++;
		}
	#if defined __APPLE2__
		wait(1); clk += 4;  // Manually update clock on Apple 2
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}
	
	// Exit bitmap mode
	ExitBitmapMode();
	
    // Done
    return EXIT_SUCCESS;	
}