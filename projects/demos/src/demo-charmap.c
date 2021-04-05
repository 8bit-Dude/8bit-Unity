
#include "unity.h"

extern const char keyNext;

extern unsigned char worldMaxX, worldMaxY;	// See: units/graphics/charmap.c

#if defined __APPLE2__
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
#elif defined __ATARI__
	unsigned char charColors[] = { 0x00, 0x0c, 0x78, 0x62, 0x12 };   // Black, White, Light Blue, Dark Blue, Red
#elif defined __CBM__
	unsigned char charColors[] = { BLACK, WHITE, LBLUE };
#elif defined __LYNX__
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
#elif defined __ORIC__
	unsigned char charColors[] = {};    //  All colors are pre-assigned in the char sheet
#endif

int DemoCharmap(void) 
{	
	unsigned char joy, scroll = 0, x = (32-CMP_COLS/2), y = (16-CMP_ROWS/2);
	
	// Prepare charmap
	InitCharmap(0, CMP_COLS, 0, CMP_ROWS-1);
	ClearCharmap();
	LoadCharset("quedex.chr", charColors);
	LoadTileset("level1.tls", 40, 2, 2);
	LoadCharmap("level1.map", 64, 32);
	ShowCharmap();
	DrawCharmap(x,y);
	
	// Print instructions
	inkColor = WHITE; paperColor = BLACK;
	txtX = 1; txtY = TXT_ROWS-1;
#if defined __LYNX__	
	PrintStr("Scroll with Joystick / OPT 1 for next");
#else
	PrintStr("Scroll with Joystick / SPACE for next");
#endif
	
	// Wait until 'SPACE' is pressed
	while (!kbhit () || cgetc () != keyNext) {	
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
		
	#if defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
	}
	
	// Clean-up memory
	HideCharmap();
	FreeCharmap();
	
    // Done
    return EXIT_SUCCESS;	
}