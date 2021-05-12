
#include "unity.h"
#include "definitions.h"

#if defined(__LYNX__)
	const char nextMsg[] = " - Press OPTION 1 - ";
	const char nextKey = 49;
#elif defined(__NES__)
	const char nextMsg[] = " - Press SELECT - ";
	const char nextKey = KB_SP;
#else
	const char nextMsg[] = " - Press SPACE bar - ";
	const char nextKey = KB_SP;
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
