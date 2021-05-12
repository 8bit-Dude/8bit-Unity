
#include "unity.h"

extern const char nextKey;

extern unsigned char worldMaxX, worldMaxY;	// See: units/graphics/charmap.c

int DemoCharmap(void) 
{	
	unsigned char joy, scroll = 0, x = (48-CMP_COLS/2), y = (24-CMP_ROWS/2);
	
	// Prepare charmap
#if defined(__APPLE2__)	|| defined(__ORIC__)
	InitCharmap(2, CMP_COLS-2, 1, CMP_ROWS-2);	// These platforms are slower (ALSO NOTE: Apple fails to allocate buffer for fullscreen... why?)
#else
	InitCharmap(0, CMP_COLS, 0, CMP_ROWS-1);	// Display field (x1,x2,y1,y2)
#endif
	LoadCharset("quedex.chr");					// Character set (contains 128 chars)
	LoadTileset("level1.tls", 40);				// Tile set (contains 40 tiles)
	LoadCharmap("level1.map", 64, 32);			// Size of map (64x32 tiles)
	ClearCharmap();
	DrawCharmap(x,y);
	ShowCharmap();
	
	// Print instructions
	inkColor = WHITE; paperColor = BLACK;
	txtX = 1; txtY = TXT_ROWS-1;
#if defined __LYNX__	
	PrintStr("Scroll with Joystick / OPT 1 for next");
#elif defined __NES__
	PrintStr("Scroll with Pad / Press SELECT");
#else
	PrintStr("Scroll with Joystick / SPACE for next");
#endif
	
	// Wait until 'SPACE' is pressed
	while (!kbhit () || cgetc () != nextKey) {	
		// Check joystick
		joy = GetJoy(0); 
		if (!(joy & JOY_UP))    { if (y > 0) 		 y -= 1; scroll = 1; }
		if (!(joy & JOY_DOWN))  { if (y < worldMaxY) y += 1; scroll = 1; }
		if (!(joy & JOY_LEFT))  { if (x > 0) 		 x -= 1; scroll = 1; }
		if (!(joy & JOY_RIGHT)) { if (x < worldMaxX) x += 1; scroll = 1; }
		
		// Update map if necessary
		if (scroll) {
			if (!(joy & JOY_BTN1))
				DrawCharmap(x, y);		// Redraw entire map
			else
				ScrollCharmap(x, y);	// Scroll and draw new tiles
			scroll = 0;
		}
		
	#if defined(__LYNX__) || defined(__NES__)
		UpdateDisplay(); // Manually refresh display
	#endif		
	}
	
	// Clean-up memory
	HideCharmap();
	FreeCharmap();
	
    // Done
    return EXIT_SUCCESS;	
}