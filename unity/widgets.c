/*
 * Copyright (c) 2020 Anthony Beaucamp.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented * you must not
 *   claim that you wrote the original software. If you use this software in a
 *   product, an acknowledgment in the product documentation would be
 *   appreciated but is not required.
 *
 *   2. Altered source versions must be plainly marked as such, and must not
 *   be misrepresented as being the original software.
 *
 *   3. This notice may not be removed or altered from any distribution.
 *
 *   4. The names of this software and/or it's copyright holders may not be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 */
 
#include "unity.h"

callback* callHead = NULL;
unsigned char countId = 0;

///////////////////////////////
// Graphical scaling functions

unsigned int ColToX(unsigned char col)
{
#if defined __APPLE2__	// DHR Mode: 140 x 192
	return (col*35)/10;
#elif defined __ATARI__	// INP Mode: 160 x 200
	return col*4;
#elif defined __ORIC__	// AIC Mode: 117 x 100 equilabelent pixels
	return col*3-3;	
#elif defined __CBM__	// MLC Mode: 160 x 200
	return col*4;
#elif defined __LYNX__	// STD Mode: 160 x 102
	return col*4;
#endif
}

unsigned int RowToY(unsigned char row)
{
#if defined __APPLE2__	// DHR Mode: 140 x 192
	return row*8;
#elif defined __ATARI__	// INP Mode: 160 x 200
	return row*8;
#elif defined __ORIC__	// AIC Mode: 117 x 100 equilabelent pixels
	return row*4;
#elif defined __CBM__	// MLC Mode: 160 x 200
	return row*8;
#elif defined __LYNX__	// STD Mode: 160 x 102
	return row*6;
#endif
}

//////////////////////////////////
/// Callback management functions

callback* CheckCallbacks(unsigned int x, unsigned int y)
{
	callback* call = callHead;
		
	// Check coordinates overlaps callback?
	while (call) {
		if (call->x1<x && x<call->x2 && call->y1<y && y<call->y2) return call;
		call = (callback*)call->next;
	}
	return 0;
}

callback* PushCallback(unsigned int col, unsigned int row, unsigned int width, unsigned int height, unsigned char type, unsigned char* label)
{
	callback* callTail = callHead;
	callback* call;
	
	// Register callback
	call = malloc(sizeof(callback));
	call->x1 = ((col)*320)/CHR_COLS;
	call->x2 = ((col+width)*320)/CHR_COLS;		
	call->y1 = ((row)*200)/CHR_ROWS;
	call->y2 = ((row+height)*200)/CHR_ROWS;
	call->id = countId++;
	call->type = type;
	call->label = label;	
	call->next = 0;
	
	// Add to callback queue
	if (!callHead) {
		callHead = call;
	} else {
		while (callTail->next) callTail = (callback*)callTail->next;
		callTail->next = (unsigned int)call;
	}
	return call;
}

void PopCallback(callback* call)
{
	callback *next, *search = callHead;
	
	// Locate the call to pop
	if (call == callHead) {
		callHead = (callback*)call->next;
		free(call);
	} else {
		while (search) {
			if (call == (callback*)search->next) {
				next = (callback*)search->next;
				search->next = next->next;
				free(call);	
				return;
			}
			search = (callback*)search->next;		
		}
	}
}

void ClearCallbacks()
{
	// Clear-out all callbacks and reset ID counter
	callback *next, *call = callHead;
	while (call) {
		next = (callback*)call->next;
		free(call);
		call = next;
	}	
	callHead = 0;
	countId = 0;
}

////////////////////////////
// Widget Drawing Functions

void Button(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* label)
{
	// Print Button
	PrintStr(col, row, label);	

	// Register Callback
	PushCallback(col, row, width, height, CALLTYPE_BUTTON, label);
}

void Panel(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title)
{
	unsigned char ink, paper;
	
	// Clear area
	PrintBlanks(col, row, width, height);
	
	// Create border
	for (pixelX=ColToX(col); pixelX<ColToX(col+width); pixelX++) {
		pixelY = RowToY(row+height)+1;
		SetPixel(inkColor); 
	}
	for (pixelY=RowToY(row); pixelY<RowToY(row+height)+2; pixelY++) {
		pixelX = ColToX(col)-1;
		SetPixel(inkColor); 
		pixelX = ColToX(col+width);
		SetPixel(inkColor); 
	}
	
	// Add Title
	ink = inkColor; paper = paperColor;
	inkColor = paper; paperColor = ink;
	PrintBlanks(col, row, width, 1);
	PrintStr(col, row, title);	
	inkColor = ink; paperColor = paper;
}
 
void ListBox(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title, unsigned char* labels[], unsigned char len)
{
	unsigned char i=0;	
	unsigned char* elt;
	
	// Draw panel
	Panel(col, row, width, height, title);
	
	// Print list and register callbacks
 	while (i<(height-1) && i<len && labels[i]) {
		elt = labels[i];
		PrintStr(col, ++row, elt);
		PushCallback(col, row, width, 1, CALLTYPE_LISTBOX, elt);
		i++;
	}
 }
 