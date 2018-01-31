
#include "test-bmp.h"
#include "test-joy.h"
#include "test-udp.h"

int main (void) 
{
	// Run all tests
	TestUDP();
	TestJOY();
	TestBMP();
	
    // Done
    return EXIT_SUCCESS;	
}