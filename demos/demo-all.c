
#include "test-bmp.h"
#include "test-gfx.h"
#include "test-joy.h"
#include "test-spr.h"
#include "test-udp.h"
//#include "test-exo.h"

int main (void) 
{
	// Run all tests
	TestBMP();
	TestGFX();
	TestJOY();
	TestSPR();
	TestUDP();
	//TestEXO();
	
    // Done
    return EXIT_SUCCESS;	
}