
#include "definitions.h"

// Load graphic chunk from file
void LoadSceneChunk(Chunk* chunk, char* filename)
{
	LoadChunk(&chunk->data, filename);
	chunk->x = chunk->data[0]; 
	chunk->y = chunk->data[1]; 
	chunk->w = chunk->data[2]; 
	chunk->h = chunk->data[3];
}

// Grab graphic chunk from screen
void GrabSceneChunk(Chunk* grab, Chunk* template)
{
	grab->x = template->x; 
	grab->y = template->y; 
	grab->w = template->w; 
	grab->h = template->h;
	GetChunk(&grab->data, grab->x, grab->y, grab->w, grab->h);
}

// Draw graphic chunk to screen
void DrawSceneChunk(Chunk* chunk)
{
	SetChunk(chunk->data, chunk->x, chunk->y);
#if defined __LYNX__
	UpdateDisplay(); // Refresh Lynx screen
#endif		
}
