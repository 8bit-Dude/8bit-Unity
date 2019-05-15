
#include "demo-hlo.h"
#include "demo-joy.h"
#include "demo-gfx.h"
#include "demo-bmp.h"
#include "demo-spr.h"
#include "demo-udp.h"

int main (void)
{
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	
	// Run all demos
	DemoHLO();
	DemoJOY();
	DemoGFX();
	DemoBMP();
	DemoSPR();
	DemoUDP();
	
    // Done
    return EXIT_SUCCESS;
}