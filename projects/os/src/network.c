
#include "definitions.h"

char netConnected = 0;

void ServerConnect()
{
	// Try connecting to 8bit-Unity server
	netConnected = !InitNetwork();
	if (netConnected) {
		SlotTCP(0);
		OpenTCP(199, 47, 196, 106, 1999);
	}
}