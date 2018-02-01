
#include "test-bmp.h"
#include "test-gfx.h"
#include "test-joy.h"
#include "test-udp.h"

int main (void) 
{
	// Run all tests
	//TestUDP();
	TestJOY();
	TestGFX();
	TestBMP();
	
    // Done
    return EXIT_SUCCESS;	
}