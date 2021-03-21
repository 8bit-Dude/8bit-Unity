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

clock_t cursorClock;
unsigned char cursorFlick, cursorLocked, cursorSel;
unsigned char cursorCol, cursorRow, cursorHeight;

void InitMenu(unsigned char col, unsigned char row, unsigned char width, unsigned char height, unsigned char padding, unsigned char **labels)
{
	unsigned char i;

	// Set cursor
	cursorCol = col;
	cursorRow = cursorSel = row;
	cursorHeight = height;
	
	// Create blank space and show options
	PrintBlanks(col-padding, row-padding, width+2*padding, height+2*padding);
	for (i=0; i<height; i++)
		PrintStr(col+2, cursorRow+i, labels[i]);
}

unsigned char UpdateMenu(unsigned char *actions)
{
	unsigned char joy;
	
	// Process keyboard inputs
	if (kbhit())
		return cgetc();

	// Periodically flick cursor
	if (clock()-cursorClock > 20) {
		cursorClock = clock();
		PrintBlanks(cursorCol, cursorRow, 2, cursorHeight);
		if (cursorFlick) {
			inkColor = YELLOW;
			PrintChr(cursorCol, cursorSel, &charHyphen[0]);
			PrintChr(cursorCol+1, cursorSel, &charBracket[3]);
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
	
	// Detect corresponding cursor key
	if (!(joy & JOY_UP)) { 
		cursorSel--; 
		if (cursorSel < cursorRow)
			cursorSel++;
	}
	if (!(joy & JOY_DOWN)) { 
		cursorSel++; 
		if (cursorSel >= cursorRow+cursorHeight)
			cursorSel--;
	}
	if (!(joy & JOY_BTN1)) { 
		return actions[cursorSel-cursorRow];
	}
	
	// No action detected
	return 0;
}
