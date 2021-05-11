/*
 * Copyright (c) 2021 Anthony Beaucamp.
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

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

#define FLICK_RATE 20 	// Ticks

clock_t cursorClock;
unsigned char cursorFlick, cursorLocked, cursorMoved, cursorPos;
unsigned char cursorCol, cursorRow, cursorHeight;
unsigned char *cursorActions;

void InitMenu(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char padding, unsigned char **labels, unsigned char *actions)
{
	unsigned char i;

	// Set cursor
	cursorCol = col;
	cursorRow = cursorPos = row;
	cursorHeight = height;
	cursorActions = actions;
	
	// Create blank space and show options
	txtX = col-padding; txtY = row-padding;
	PrintBlanks(width+2*padding, height+2*padding);
	txtX = col+2; txtY = cursorRow;
	for (i=0; i<height; i++) {
		PrintStr(labels[i]);
		txtY++;
	}
}

unsigned char UpdateMenu(void)
{
	unsigned char joy;

	// Reset move flag
	cursorMoved = 0;
	
	// Process keyboard inputs
	if (kbhit())
		return cgetc();

	// Periodically flick cursor
	if (clock()-cursorClock > FLICK_RATE) {
		cursorClock = clock();
		txtX = cursorCol; txtY = cursorRow;
		PrintBlanks(2, cursorHeight);
		if (cursorFlick) {
			inkColor = YELLOW;
			txtY = cursorPos;
			txtX = cursorCol; PrintChr('-');
			txtX++;			  PrintChr('>');
			inkColor = WHITE;
		}
		cursorFlick = !cursorFlick;
	}
	
	// Check if cursor is locked
	joy = GetJoy(0);
	if (~joy & 0b00011111) {
		if (cursorLocked) 
			return 0;
		cursorLocked = 1;
		cursorFlick = 1;
		cursorClock = 0;
	} else { 
		cursorLocked = 0; 
	}
	
	// Detect cursor motion
	if (!(joy & JOY_UP)) { 
		//while (!cursorActions[--cursorPos]);
		--cursorPos;
		cursorMoved = 1;
	}
	if (!(joy & JOY_DOWN)) { 
		//while (!cursorActions[++cursorPos]);
		++cursorPos;
		cursorMoved = 1;
	}
	
	// Check limits
	if (cursorPos < cursorRow)
		cursorPos = cursorRow+cursorHeight-1;
	if (cursorPos >= cursorRow+cursorHeight)
		cursorPos = cursorRow;
	
	// Detect cursor action
	if (!(joy & JOY_BTN1)) { 
		return cursorActions[cursorPos-cursorRow];
	}
	
	// No action detected
	return 0;
}
