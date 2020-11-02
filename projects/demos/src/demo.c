
#include "demo-hlo.h"
#include "demo-joy.h"
#include "demo-net.h"
#include "demo-bmp.h"
#include "demo-gfx.h"
#include "demo-spr.h"

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
	DemoHLO();
	DemoJOY();
	DemoNET();
	DemoBMP();
	DemoGFX();
	DemoSPR();
	
    // Done
    return EXIT_SUCCESS;
}