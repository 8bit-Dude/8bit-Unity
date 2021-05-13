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

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

callback* callHead = NULL;
callback* callList = NULL;

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

unsigned long fraction;

unsigned char SliderPos(callback* call)
{
	fraction  = *(unsigned int*)&call->buffer[0];	
	fraction *= call->rowEnd - call->rowBeg - 2;
	fraction /= *(unsigned int*)&call->buffer[2]; 
	return (call->rowBeg + fraction + 1);
}

#if defined __LYNX__ 
  #define kbhit KeyboardOverlayHit
  #define cgetc GetKeyboardOverlay
#endif

callback *inputCall; 
unsigned char inputWidth;

unsigned char ProcessInput()
{
	if (!inputCall)
		return 0;
	
	if (kbhit()) {
		inkColor = inputCall->ink; paperColor = inputCall->paper;		
		txtX = inputCall->colBeg; txtY = inputCall->rowBeg;
		if (InputStr(inputWidth, inputCall->buffer, inputCall->value, cgetc())) {
			inputCall = 0;
		#if defined __LYNX__
			HideKeyboardOverlay();	
		#endif	
			return 0;		
		}			
	}
	return 1;
}

//////////////////////////////////
/// Callback management functions

callback* CheckCallbacks(unsigned char col, unsigned char row)
{
	callback *call = callHead;
	unsigned int val, max;
		
	// Check coordinates overlaps callback?
	while (call) {
		if (call->colBeg<=col && col<call->colEnd && call->rowBeg<=row && row<call->rowEnd) {
			// Trigger click action (if any)
			switch (call->type) {
			case CALLTYPE_INPUT:	
				inputCall = call;
				inputWidth = call->colEnd - call->colBeg;
			#if defined __LYNX__ 
				SetKeyboardOverlay(54,70);
				ShowKeyboardOverlay();
			#endif		
				break;
				
			case CALLTYPE_LISTBOX:
				// Change highlight to new item
				if (callList) {
					inkColor = callList->ink;
					paperColor = callList->paper;
					txtX = callList->colBeg;
					txtY = callList->rowBeg;
					PrintStr(callList->buffer);
				}
				inkColor = call->paper;
				paperColor = call->ink;
				txtX = call->colBeg;
				txtY = call->rowBeg;
				PrintStr(call->buffer);
				callList = call;
				break;

			case CALLTYPE_SCROLLBAR:
				// Update position of slider
				inkColor = call->ink;
				paperColor = call->paper;
				txtX = call->colBeg;
				txtY = SliderPos(call);
				PrintChr(CHR_LINE_VERT);
				val = *(unsigned int*)&call->buffer[0];
				max = *(unsigned int*)&call->buffer[2];
				if (row == call->rowBeg) {
					if (val > 0) 
						(val)--;
				} else 
				if (row == call->rowEnd-1) {
					if (val < max-1)
						(val)++;
				} else 
				if (row == call->rowBeg+1) {
					val = 0;
				} else 
				if (row == call->rowEnd-2) {
					val = max-1;
				} else {
					val  = max;
					val *= row - call->rowBeg - 1;
					val /= call->rowEnd - call->rowBeg - 2;
				}
				*(unsigned int*)&call->buffer[0] = val;
				txtY = SliderPos(call);
				PrintChr(CHR_SLIDER_VERT);
				break;
				
			}
			return call;
		}
		call = (callback*)call->next;
	}
	return 0;
}

callback* PushCallback(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char type)
{
	callback* callTail = callHead;
	callback* call;
	
	// Register callback
	call = malloc(sizeof(callback));
	call->colBeg = col;
	call->colEnd = (col+width);		
	call->rowBeg = row;
	call->rowEnd = (row+height);
	call->ink    = inkColor;
	call->paper  = paperColor;
	call->type   = type;	
	call->next   = 0;
	
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
	callList = NULL;
}

////////////////////////////
// Widget Drawing Functions

callback* Button(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* label)
{
	// Print Button
	txtX = col; txtY = row;
	PrintStr(label);	

	// Register Callback
	return PushCallback(col, row, width, height, CALLTYPE_BUTTON);
}

callback* Icon(unsigned char col, unsigned char row, unsigned char* chunk)
{
	unsigned char width, height;
	
	// Display chunk at desired position
#if defined __ORIC__
	SetChunk(chunk, 2*ColToX(col), 2*RowToY(row));
	width = XToCol(chunk[2])/2; height = YToRow(chunk[3])/2;
#else
	SetChunk(chunk, ColToX(col), RowToY(row));
	width = XToCol(chunk[2]); height = YToRow(chunk[3]);
#endif
	
	// Register Callback
	return PushCallback(col, row, width, height, CALLTYPE_ICON);	
}

callback* Input(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* buffer, unsigned char len)
{
	callback* call;
		
	// Clear Field Area
	txtX = col; txtY = row;
	InputStr(width, buffer, len, 0);

	// Register Callback
	call = PushCallback(col, row, width, height, CALLTYPE_INPUT);
	call->buffer = buffer;
	call->value = len;
	return call;
}

void Panel(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title)
{
	unsigned char xC, xCW, yR, yRH;
	unsigned char ink, paper;
	
	// Clear area
	txtX = col; txtY = row;
	PrintBlanks(width, height);
	
	// Create border
	xC = ColToX(col); xCW = ColToX(col+width);
	yR = RowToY(row); yRH = RowToY(row+height); 
	Line(xC, xCW, yRH+1, yRH+1);
	Line(xC-1, xC-1, yR, yRH+2);
	Line(xCW, xCW, yR, yRH+2);
	
	// Add Title
	ink = inkColor; paper = paperColor;
	inkColor = paper; paperColor = ink;
	PrintBlanks(width, 1);
	PrintStr(title);	
	inkColor = ink; paperColor = paper;
}

void Line(unsigned char x1, unsigned char x2, unsigned char y1, unsigned char y2)
{
#ifndef __NES__	
	if (x1 == x2) {
		pixelX = x1;
		for (pixelY=y1; pixelY<y2; pixelY++)
			SetPixel(inkColor); 
	} else {
		pixelY = y1;
		for (pixelX=x1; pixelX<x2; pixelX++)
			SetPixel(inkColor); 
	}
#endif
}

void ListBox(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char* title, unsigned char* labels[], unsigned char len)
{
	unsigned char i=0;	
	unsigned char* elt;
	callback* call;
	
	// Draw panel
	Panel(col, row, width, height, title);
	
	// Print list and register callbacks
	txtX = col; txtY = row+1;
 	while (i<(height-1) && i<len && labels[i]) {
		elt = labels[i];
		PrintStr(elt);
		call = PushCallback(txtX, txtY, width, 1, CALLTYPE_LISTBOX);
		call->buffer = elt;
		call->value = i;
		txtY++; i++;
	}
}

callback* ScrollBar(unsigned char col, unsigned char row, unsigned char height, unsigned int *range)
{
	callback* call;
	unsigned char i=0;	

	// Draw scrollbar
	txtX = col; txtY = row;
	PrintChr(CHR_ARROW_UP);
	while (++txtY < (row+height-1)) {
		PrintChr(CHR_LINE_VERT);
	}
	PrintChr(CHR_ARROW_DOWN);
	
	// Register callback
	call = PushCallback(col, row, 1, height, CALLTYPE_SCROLLBAR);
	call->buffer = (unsigned char*)range;

	// Show slider
	txtY = SliderPos(call);
	PrintChr(CHR_SLIDER_VERT);
	return call;
}
