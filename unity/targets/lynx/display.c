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

// See suzy.s
unsigned char videoInit = 0;	 
void __fastcall__ SuzyInit(void);
void __fastcall__ SuzyDraw(void* data);
void __fastcall__ SuzyUpdate(void);
unsigned char __fastcall__ SuzyBusy(void);

// See keyboard.c
void UpdateKeyboardOverlay(void);

// Toggle for automatic screen refresh in LoadBitmap(), PrintStr(), PrintNum() or PrintBlanks()
unsigned char autoRefresh = 1;

// See sprites.c
extern SCB_REHV_PAL sprSCB[SPRITE_NUM];
extern unsigned char sprDrawn[SPRITE_NUM];

// GFX Data 
static char palette[] =    { 0x01, 0x04, 0x08, 0x0c, 0x0c, 0x08, 0x05, 0x05, 0x07, 0x0e, 0x09, 0x03, 0x02, 0x0f, 0x08, 0x00,	// Upper 8 bits
							 0xca, 0xb4, 0xc4, 0xc3, 0x53, 0x22, 0x52, 0x27, 0x5e, 0x0f, 0xaf, 0x4d, 0x48, 0xff, 0x88, 0x00 };  // Lower 8 bits
SCB_REHV_PAL bitmapSCB =  { BPP_4 | TYPE_BACKNONCOLL, REHV | LITERAL, 0, 0, (char*)BITMAPRAM, 0, 0, 0x0100, 0x0100, { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef } };

void InitDisplay(void)
{
	// Did we already initialize?
	if (videoInit) { return; }
	
	// Install drivers (and set interrupts)
	SuzyInit();
	lynx_snd_init();
	__asm__("cli");
	
	// Set palette and init flag
	memcpy(0xFDA0, palette, 32);
	videoInit = 1;
}

// Update display routine
void UpdateDisplay(void)
{
	unsigned char i;

	// Wait for previous drawing to complete
	while (SuzyBusy()) {}
	
	// Draw bitmap
	SuzyDraw(&bitmapSCB);
	
	// Draw sprites (in reverse order)
	i = SPRITE_NUM;
	while (i>0) {
		i--; 
		if (sprDrawn[i]) 
			SuzyDraw(&sprSCB[i]);
	}
	
	// Draw soft keyboard?
	//UpdateKeyboardOverlay();
	
	// Switch buffer frame
	SuzyUpdate();
}
