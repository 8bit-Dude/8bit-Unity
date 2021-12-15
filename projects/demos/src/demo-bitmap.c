
#include "unity.h"

#if defined __APPLE2__
  const unsigned char coords[3][4] = { {  7, 5, 28, 39 }, { 56, 5, 28, 39 }, {105, 5, 28, 39} };
#elif defined __ATARI__
  const unsigned char coords[3][4] = { {  8, 6, 36, 40 }, { 60, 6, 36, 40 }, {112, 6, 36, 40} };
#elif defined __C64__
  const unsigned char coords[3][4] = { {  8, 0, 36, 48 }, { 60, 0, 36, 48 }, {112, 0, 36, 48} };
#elif defined __LYNX__
  const unsigned char coords[3][4] = { { 12, 3, 30, 20 }, { 64, 3, 30, 20 }, {116, 3, 30, 20} };
#elif defined __NES__
  const unsigned char coords[3][4] = { { 16, 0, 48, 48 }, {104, 0, 48, 48 }, {192, 0, 48, 48} };
#elif defined __ORIC__
  const unsigned char coords[3][4] = { { 18, 4, 48, 42 }, { 96, 4, 48, 42 }, {174, 4, 48, 42} };
#endif

extern const char nextKey;

int DemoBitmap(void) 
{
	unsigned char i, r;
	unsigned char* chunks[3];
	clock_t time = clock();
	
	// Prepare bitmap
	InitBitmap();
	LoadBitmap("banner.img");
	ShowBitmap();

	// Grab graphic chunks
	for (i=0; i<3; i++) 
		GetChunk(&chunks[i], coords[i][0], coords[i][1], coords[i][2], coords[i][3]);

	// Wait until 'SPACE' is pressed
	while (!kbhit () || cgetc () != nextKey) {	
		if (clock() > time+4) {
			// Rotate chunks
			time = clock();
			for (i=0; i<3; i++) 
				SetChunk(chunks[(i+r)%3], coords[i][0], coords[i][1]);
			r++;
		}
	#if defined __APPLE2__
		wait(1); clk += 4;  // Manually update clock on Apple 2
	#elif defined(__LYNX__) || defined(__NES__)
		UpdateDisplay(); 	// Manually refresh Display
	#endif		
	}
	
	// Exit bitmap mode
	HideBitmap();
	
	// Clean-up memory
#ifndef __NES__	
	for (i=0; i<3; i++) 
		free(chunks[i]);
#endif

    // Done
    return EXIT_SUCCESS;	
}
