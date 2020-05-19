
#include "definitions.h"

#define MAX_CHUNK 6
unsigned char* chunks[MAX_CHUNK];

// Grab graphic chunk from screen
void GrabChunkBG(unsigned char** grab, unsigned char* template)
{
	GetChunk(grab, template[0], template[1], template[2], template[3]);
}

// Draw graphic chunk to screen
void DrawChunk(unsigned char* chunk)
{
	SetChunk(chunk, chunk[0], chunk[1]);
#if defined __LYNX__
	UpdateDisplay(); // Refresh Lynx screen
#endif		
}
