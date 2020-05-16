
#include "definitions.h"

// Load graphic chunk from file
void LoadSceneChunk(Chunk* chunk, unsigned char x, unsigned char y, unsigned char w, unsigned char h, char* filename)
{
	chunk->x = x; chunk->y = y; chunk->w = w; chunk->h = h;
	LoadChunk(&chunk->data, filename, chunk->w, chunk->h);
}

// Grab graphic chunk from screen
void GrabSceneChunk(Chunk* chunk, unsigned char x, unsigned char y, unsigned char w, unsigned char h)
{
	chunk->x = x; chunk->y = y; chunk->w = w; chunk->h = h;
	GetChunk(&chunk->data, chunk->x, chunk->y, chunk->w, chunk->h);
}

// Draw graphic chunk to screen
void DrawSceneChunk(Chunk* chunk)
{
	SetChunk(chunk->data, chunk->x, chunk->y, chunk->w, chunk->h);
#if defined __LYNX__
	UpdateDisplay(); // Refresh Lynx screen
#endif		
}
