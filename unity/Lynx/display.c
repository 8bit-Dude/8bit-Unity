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
 
#include "../unity.h"

// See suzy.s
unsigned char videoInit = 0;	 
void __fastcall__ SuzyInit(void);
void __fastcall__ SuzyDraw(void* data);
void __fastcall__ SuzyUpdate(void);
unsigned char __fastcall__ SuzyBusy(void);

// Toggle for automatic screen refresh in LoadBitmap(), PrintStr(), PrintNum() or PrintBlanks()
unsigned char autoRefresh = 1;

// See sprites.c
extern SCB_REHV_PAL sprSCB[SPRITE_NUM];
extern unsigned char sprDrawn[SPRITE_NUM];

// GFX Data
extern unsigned int charData[];	 
extern unsigned int cursorData, keybrdData; 
static int palette[] =    { 0x01ca, 0x03b4, 0x08c4, 0x0cc3, 0x0c53, 0x0822, 0x0552, 0x0527, 
						    0x075e, 0x0e0f, 0x09af, 0x034d, 0x0248, 0x0fff, 0x0888, 0x0000 };	// Set RGB values of 16 color palette
SCB_REHV_PAL bitmapSCB =  { BPP_4 | TYPE_BACKNONCOLL, REHV | LITERAL, 0, 0, (char*)BITMAPRAM, 0, 0, 
						    0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } };
SCB_REHV_PAL charmapSCB = { BPP_4 | TYPE_BACKNONCOLL, REHV | LITERAL, 0, 0, 0, 0, 0, 
						    0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } };
SCB_REHV_PAL cursorSCB =  { BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, (char*)&cursorData, 0, 0, 
						    0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } };							
SCB_REHV_PAL keybrdSCB =  { BPP_4 | TYPE_NONCOLL, REHV | LITERAL, 0, 0, (char*)&keybrdData, 0, 0, 
						    0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } };							

// Workaround for missing char printing (including palette remapping)
unsigned char textColors[] = { BLACK, RED, PINK, GREY, GREY, GREY, BROWN, ORANGE, YELLOW, LGREEN, GREEN, DRED, PURPLE, BLUE, LBLUE, WHITE };
unsigned char chrCol, chrRow;
void clrscr(void) {
	InitBitmap(); ClearBitmap();
}
unsigned char textcolor(unsigned char color) {
	inkColor = textColors[color];
}
unsigned char bgcolor(unsigned char color) {
	paperColor = textColors[color];
}
unsigned char bordercolor(unsigned char color) {
}
void screensize(unsigned char *xSize, unsigned char *ySize) {
	*xSize = 40; *ySize = 17;
}
void gotoxy(unsigned char col, unsigned char row) {
	chrCol = col; chrRow = row;
}
int cprintf (const char* format, ...) {
	PrintStr(chrCol, chrRow, format);
	chrCol += strlen(format);
	while (chrCol > 39) {
		chrCol -= 40;
		chrRow++;
	}
}

// Soft keyboard functions
clock_t keybrdClock = 0;
unsigned char keybrdShow = 0, keybrdVal = 0; 
unsigned char keybrdJoy = 0, keybrdPressed = 0;
signed char keybrdRow = 0, keybrdCol = 0;
signed char keyCodes[4][13] = { { 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 95, 43 },
								{ 81, 87, 69, 82, 84, 89, 85, 73, 79, 80, 40, 41, 20 },
								{ 65, 83, 68, 70, 71, 72, 74, 75, 76, 58, 39, 33, 13 },
								{ 90, 88, 67, 86, 66, 78, 77, 44, 46, 63, 47, 92, 32 } };
void ShowKeyboardOverlay() {
	keybrdShow = 1;
	keybrdPressed = 1;
}
void HideKeyboardOverlay() {
	keybrdShow = 0;
}
void SetKeyboardOverlay(unsigned char x, unsigned char y) {
	keybrdSCB.hpos = x;
	keybrdSCB.vpos = y;
}
unsigned char GetKeyboardOverlay() {
	unsigned char val = keybrdVal;
	keybrdVal = 0;
	return val;
}
unsigned char KeyboardOverlayHit() {
	return keybrdVal;
}
void UpdateKeyboardOverlay() {
	// Is keyboard activated?
	if (!keybrdShow) { return; }

	// Check if cursor was already pressed
	keybrdJoy = GetJoy(0);
	if (keybrdJoy != 255) {
		// Process next event
		if (!keybrdPressed) {  
			keybrdPressed = 1;
			keybrdClock = clock()-20;
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
	if (clock()-keybrdClock > 40) { 
		keybrdClock = clock();	 
	} else if (clock()-keybrdClock > 20) {
		cursorSCB.hpos = (keybrdSCB.hpos+1) + keybrdCol*4;
		cursorSCB.vpos = (keybrdSCB.vpos+1) + keybrdRow*6;
		SuzyDraw(&cursorSCB);
	}	
}

void InitDisplay(void)
{
	// Did we already initialize?
	unsigned char i;
	if (videoInit) { return; }
	
	// Install drivers (and set interrupts)
	SuzyInit();
	lynx_snd_init();
	__asm__("cli");	
	
	// Reset palette
	for (i=0; i<16; i++) {
		POKE(0xFDA0+i, palette[i] >> 8);
		POKE(0xFDB0+i, palette[i] & 0xFF);
	}	
	
	// Set flag
	videoInit = 1;
}

// Update display routine
void UpdateDisplay(void)
{
	unsigned char i, j;

	// Wait for previous drawing to complete
	while (SuzyBusy()) {}
	
	// Draw bitmap
	SuzyDraw(&bitmapSCB);
	
	// Draw sprites (in reverse order)
	for (j=0; j<SPRITE_NUM; j++) {
		i = (SPRITE_NUM-1) - j;
		if (sprDrawn[i]) SuzyDraw(&sprSCB[i]);
	}
	
	// Draw soft keyboard?
	UpdateKeyboardOverlay();
	
	// Switch buffer frame
	SuzyUpdate();
}
