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
#elif defined __ORIC__	// AIC Mode: 117 x 100 equivalent pixels
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
#elif defined __ORIC__	// AIC Mode: 117 x 100 equivalent pixels
	return row*4;
#elif defined __CBM__	// MLC Mode: 160 x 200
	return row*8;
#elif defined __LYNX__	// STD Mode: 160 x 102
	return row*6;
#endif
}

//////////////////////////////////
/// Callback management functions

callback* CheckCallbacks(unsigned char col, unsigned char row)
{
	callback* call = callHead;
		
	// Check coordinates overlaps callback?
	while (call) {
		if (call->colBeg<=col && col<call->colEnd && call->rowBeg<=row && row<call->rowEnd) return call;
		call = (callback*)call->next;
	}
	return 0;
}

callback* PushCallback(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char type, unsigned char* label)
{
	callback* callTail = callHead;
	callback* call;
	
	// Register callback
	call = malloc(sizeof(callback));
	call->colBeg = col;
	call->colEnd = (col+width);		
	call->rowBeg = row;
	call->rowEnd = (row+height);
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

callback* Button(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* label)
{
	// Print Button
	PrintStr(col, row, label);	

	// Register Callback
	return PushCallback(col, row, width, height, CALLTYPE_BUTTON, label);
}

callback* Input(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* buffer)
{
	// Clear Field Area
	PrintBlanks(col, row, width, height);
	PrintStr(col, row, buffer);

	// Register Callback
	return PushCallback(col, row, width, height, CALLTYPE_INPUT, buffer);
}

void Panel(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title)
{
	unsigned char xC, xCW, yR, yRH;
	unsigned char ink, paper;
	
	// Clear area
	PrintBlanks(col, row, width, height);
	
	// Create border
	xC = ColToX(col); xCW = ColToX(col+width);
	yR = RowToY(row); yRH = RowToY(row+height); 
	Line(xC, xCW, yRH+1, yRH+1);
	Line(xC-1, xC-1, yR, yRH+2);
	Line(xCW, xCW, yR, yRH+2);
	
	// Add Title
	ink = inkColor; paper = paperColor;
	inkColor = paper; paperColor = ink;
	PrintBlanks(col, row, width, 1);
	PrintStr(col, row, title);	
	inkColor = ink; paperColor = paper;
}

void Line(unsigned char x1, unsigned char x2, unsigned char y1, unsigned char y2)
{
	if (x1 == x2) {
		pixelX = x1;
		for (pixelY=y1; pixelY<y2; pixelY++)
			SetPixel(inkColor); 
	} else {
		pixelY = y1;
		for (pixelX=x1; pixelX<x2; pixelX++)
			SetPixel(inkColor); 
	}
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

callback* ScrollBar(unsigned char col, unsigned char row, unsigned char height, unsigned char value, unsigned char* name)
{
	unsigned char i=0;	

	// Draw scrollbar and register callback
	PrintChr(col, row, charArrowUp);
	while (++i<height)
		PrintChr(col, row+i, charLineVert);
	PrintChr(col, row+value+1, charSliderVert);
	PrintChr(col, row+i, charArrowDown);
	PushCallback(col, row, 1, height, CALLTYPE_SCROLLBAR, name);	
}
