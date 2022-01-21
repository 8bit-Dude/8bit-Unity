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

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

// Load charset and associated attributes / flags
#if defined __NES__
  extern unsigned char fileIndex;
#endif
void LoadCharset(char* filename)
{
#if defined __APPLE2__
	if (FileOpen(filename)) {
	  #if defined __DHR__	
		FileRead((char*)charsetData, 0x1080);
	  #else	
		FileRead((char*)charsetData, 0x0880);
	  #endif		
		FileClose();
	}
#elif defined __ATARI__
	if (FileOpen(filename)) {
		FileRead((char*)chrPalette,  0x0005);	// Shared Colors
		FileRead((char*)charsetData, 0x0400);	// Pixel Data
		FileRead((char*)charatrData, 0x0100);	// 5th Color Attribute (0 or 128)
	}
#elif defined __CBM__	
	if (FileOpen(filename)) {	
		FileRead((char*)0xd021, 0x0003);		// Shared Colors
		FileRead((char*)charsetData, 0x0400);	// Pixel Data
		FileRead((char*)charatrData, 0x0100);	// Color Attributes
		FileClose();		
	}
#elif defined __LYNX__
	if (FileOpen(filename)) {	// Data is loaded into BITMAP memory on open
		memcpy(charsetData, BITMAPRAM, 0x0480);
	}
#elif defined __NES__
	if (FileOpen(filename)) {
		// Copy palette data
		FileRead(charflagData, -1);
		memcpy(palBG, &charflagData[0x80], 4);
		
		// Switch to new palette/charset
		ppu_off();			
		pal_bg(palBG);
		set_chr_bank_0(2+fileIndex);
		ppu_on_all();
	}
#elif defined __ORIC__
	if (FileOpen(filename)) {
		FileRead(charsetData, 0x0480);
	}
#endif			
}
