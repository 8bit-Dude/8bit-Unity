
#include "demo-bmp.h"
#include "demo-gfx.h"
#include "demo-joy.h"
#include "demo-spr.h"
#include "demo-udp.h"
//#include "demo-exo.h"

int main (void) 
{
	// Run all demos
	DemoBMP();
	DemoGFX();
	DemoJOY();
	DemoSPR();
	DemoUDP();
	//DemoEXO();
	
    // Done
    return EXIT_SUCCESS;	
}