
#include "unity.h"
#include "definitions.h"

#ifdef __LYNX__
	const char pressKeyMsg[] = " - press OPTION 1 - ";
	const char keyNext = 49;
#else
	const char pressKeyMsg[] = " - press SPACE bar - ";
	const char keyNext = KB_SP;
#endif

int main (void)
{
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	
	// Run all demos
	DemoHello();
	DemoControls();
	DemoNetwork();
	DemoGfx();
	DemoBitmap();
	DemoCharmap();
	DemoSprites();
	
    // Done
    return EXIT_SUCCESS;
}
