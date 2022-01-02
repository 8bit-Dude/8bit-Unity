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
 
#include "../../unity.h"

#pragma rodata-name("BANK0")
#pragma code-name("BANK0")

// Soft keyboard functions
#pragma bss-name(push, "XRAM")
  clock_t keybrdClock = 0;
  unsigned char keybrdShow = 0, keybrdVal = 0, keybrdPressed = 0;
  unsigned char keybrdX = 0, keybrdY = 0;
  signed char keybrdRow = 2, keybrdCol = 12;
  unsigned char keybrdJoy;
  char* keybrdBG;
#pragma bss-name(pop)

const signed char keyCodes[4][13] = { { 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 95, 43 },
									  { 81, 87, 69, 82, 84, 89, 85, 73, 79, 80, 40, 41, 20 },
									  { 65, 83, 68, 70, 71, 72, 74, 75, 76, 58, 39, 33, 13 },
									  { 90, 88, 67, 86, 66, 78, 77, 44, 46, 63, 47, 92, 32 } };
									  
extern unsigned char *chunkPtr;
extern unsigned char chunkBuf[];

unsigned char SwapSpecialChar(unsigned char in)
{
	if (in == 20) 
		return 66;
	if (in == 13)
		return 82;
	return in;
}
									  
void ShowKeyboardOverlay() 
{
	unsigned char i,j;
	
	// Use chunks to save background
	GetChunk(&keybrdBG, keybrdX*8, keybrdY*8, 15*8, 4*8);
	
	// Display Keyboard
	BackupCursor();
	txtY = keybrdY;
	for (j=0; j<4; j++) {
		txtX = keybrdX; PrintChr(' ');
		for (i=0; i<13; i++) {
			txtX++; PrintChr(SwapSpecialChar(keyCodes[j][i]));
		}
		txtX++; PrintChr(' ');
		txtY++;
	}
	RestoreCursor();
	
	// Set cursor state
	keybrdShow = 1;
	keybrdPressed = 1;
}
void HideKeyboardOverlay() 
{
	// Restore background
	SetChunk(keybrdBG, keybrdX*8, keybrdY*8);
	chunkPtr = chunkBuf;  // Reset Chunk Buffer
	keybrdShow = 0;
}
void SetKeyboardOverlay(unsigned char x, unsigned char y) 
{
	keybrdX = x/5u;
	keybrdY = y/4u;
}

unsigned char keyCur;

unsigned char GetKeyboardOverlay() 
{
	keyCur = keybrdVal;
	keybrdVal = 0;
	return keyCur;
}
unsigned char KeyboardOverlayHit() 
{
	return keybrdVal;
}

void DisplayChar(unsigned char chr)
{
	BackupCursor();
	txtX = keybrdX+keybrdCol+1;
	txtY = keybrdY+keybrdRow;
	PrintChr(SwapSpecialChar(chr));
	RestoreCursor();
}

void UpdateKeyboardOverlay() 
{
	// Is keyboard activated?
	if (!keybrdShow) { return; }
	
	// Get current char
	keyCur = keyCodes[keybrdRow][keybrdCol];

	// Check if cursor was already pressed
	keybrdJoy = GetJoy(0);
	if (keybrdJoy != 255) {
		// Process next event
		if (!keybrdPressed) {  
			DisplayChar(keyCur);
			keybrdPressed = 1;
			keybrdClock = clock();
			if (!(keybrdJoy & JOY_LEFT)) { keybrdCol -= 1; if (keybrdCol<0)  keybrdCol = 12; }
			if (!(keybrdJoy & JOY_RIGHT)){ keybrdCol += 1; if (keybrdCol>12) keybrdCol = 0; }
			if (!(keybrdJoy & JOY_UP))   { keybrdRow -= 1; if (keybrdRow<0)  keybrdRow = 3; }
			if (!(keybrdJoy & JOY_DOWN)) { keybrdRow += 1; if (keybrdRow>3)  keybrdRow = 0; }
			if (!(keybrdJoy & JOY_BTN1)) { keybrdVal = keyCur; }
			if (!(keybrdJoy & JOY_BTN2)) { keybrdVal = CH_DEL; }
		}
	} else { 
		// Flag joy release
		keybrdPressed = 0; 
	}
	
	// Cursor animation
	if (clock()-keybrdClock > 40) { 
		keybrdClock = clock();	 
	} else {
		// Flicker current char
		if (clock()-keybrdClock > 20) {
			DisplayChar(keyCur);
		} else {
			DisplayChar(32);
		}
	}
}
