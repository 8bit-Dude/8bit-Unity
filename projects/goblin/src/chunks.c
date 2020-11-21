
#include "definitions.h"

// Grab graphic chunk from screen
void GrabBackground(unsigned char** bcgr, unsigned char* anim)
{
	GetChunk(bcgr, anim[0], anim[1], anim[2], anim[3]);
}

// Draw graphic chunk to screen
void DrawChunk(unsigned char* chunk)
{
	SetChunk(chunk, chunk[0], chunk[1]);
#if defined __LYNX__
	UpdateDisplay(); // Refresh Lynx screen
#endif		
}
