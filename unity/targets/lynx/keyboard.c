/*
 * Copyright (c) 2018 Anthony Beaucamp.
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

void __fastcall__ SuzyDraw(void* data);

extern unsigned char cursorData, keybrdData; 

SCB_REHV_PAL cursorSCB =  { BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, (char*)&cursorData, 0, 0, 
						    0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } };			
							
SCB_REHV_PAL keybrdSCB =  { BPP_4 | TYPE_NONCOLL, REHV | PACKED, 0, 0, (char*)&keybrdData, 0, 0, 
						    0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } };							

// Soft keyboard functions
clock_t keybrdClk = 0;
unsigned char keybrdShow = 0, keybrdVal = 0; 
unsigned char keybrdJoy = 0, keybrdPressed = 0;
signed char keybrdRow = 2, keybrdCol = 12;
signed char keyCodes[4][13] = { { 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 95, 43 },
								{ 81, 87, 69, 82, 84, 89, 85, 73, 79, 80, 40, 41, 20 },
								{ 65, 83, 68, 70, 71, 72, 74, 75, 76, 58, 39, 33, 13 },
								{ 90, 88, 67, 86, 66, 78, 77, 44, 46, 63, 47, 92, 32 } };

void ShowKeyboardOverlay() 
{
	keybrdShow = 1;
	keybrdPressed = 1;
}
void HideKeyboardOverlay() 
{
	keybrdShow = 0;
}
void SetKeyboardOverlay(unsigned char x, unsigned char y) 
{
	keybrdSCB.hpos = x;
	keybrdSCB.vpos = y;
}
unsigned char GetKeyboardOverlay() 
{
	unsigned char val = keybrdVal;
	keybrdVal = 0;
	return val;
}
unsigned char KeyboardOverlayHit() 
{
	return keybrdVal;
}

void UpdateKeyboardOverlay() 
{
	// Is keyboard activated?
	if (!keybrdShow) { return; }

	// Check if cursor was already pressed
	keybrdJoy = GetJoy(0);
	if (keybrdJoy != 255) {
		// Process next event
		if (!keybrdPressed) {  
			keybrdPressed = 1;
			keybrdClk = clock()-20;
			if (!(keybrdJoy & JOY_LEFT)) { keybrdCol -= 1; if (keybrdCol<0)  keybrdCol = 12; }
			if (!(keybrdJoy & JOY_RIGHT)){ keybrdCol += 1; if (keybrdCol>12) keybrdCol = 0; }
			if (!(keybrdJoy & JOY_UP))   { keybrdRow -= 1; if (keybrdRow<0)  keybrdRow = 3; }
			if (!(keybrdJoy & JOY_DOWN)) { keybrdRow += 1; if (keybrdRow>3)  keybrdRow = 0; }
			if (!(keybrdJoy & JOY_BTN1)) { keybrdVal = keyCodes[keybrdRow][keybrdCol]; }
			if (!(keybrdJoy & JOY_BTN2)) { keybrdVal = CH_DEL; }
		}
	} else { 
		// Flag joy release
		keybrdPressed = 0; 
	}
	
	// Draw keyboard and cursor
	SuzyDraw(&keybrdSCB);
	if (clock()-keybrdClk > 40) { 
		keybrdClk = clock();	 
	} else if (clock()-keybrdClk > 20) {
		cursorSCB.hpos = (keybrdSCB.hpos+1) + keybrdCol*4;
		cursorSCB.vpos = (keybrdSCB.vpos+1) + keybrdRow*6;
		SuzyDraw(&cursorSCB);
	}	
}
