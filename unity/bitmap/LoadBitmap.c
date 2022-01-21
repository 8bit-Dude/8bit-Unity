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
  #pragma bss-name(push, "XRAM")
  unsigned char rleData[BITMAPRAM];  
  #pragma bss-name(pop)
  extern unsigned char fileIndex;
#endif

// Load bitmap from file
void LoadBitmap(char *filename) 
{
#if defined __APPLE2__
  #if defined __DECRUNCH__	
	unsigned int size;
	if (FileOpen(filename)) {
	  #if defined __DHR__	
		FileRead((char*)&size, 2);
		*dhraux = 0;  
		FileRead((char*)BITMAPRAM-8, size);	// Read and decrunch to AUX
		Decrunch(BITMAPRAM-8+size);		
		*dhrmain = 0; 
	  #endif
		FileRead((char*)&size, 2);
		FileRead((char*)BITMAPRAM-8, size);	// Read and decrunch to MAIN
		Decrunch(BITMAPRAM-8+size);		
		FileClose();
	}
  #else
	if (FileOpen(filename)) {
	  #if defined __DHR__	
		*dhraux = 0;  
		FileRead((char*)BITMAPRAM, 8192); // Read 8192 bytes to AUX
		*dhrmain = 0; 
	  #endif
		FileRead((char*)BITMAPRAM, 8192); // Read 8192 bytes to MAIN
		FileClose();
	}	  
  #endif
  
#elif defined __ATARI__
  #if defined __DECRUNCH__	
	unsigned int size;
	if (FileOpen(filename)) {		
		FileRead(bmpPalette, 4);				// 4 bytes palette
		FileRead((char*)&size, 2);
		FileRead((char*)BITMAPRAM1-8, size);	// Read and decrunch frame 1
		Decrunch(BITMAPRAM1-8+size);	
	  #ifdef __ATARIXL__	
		FileRead((char*)&size, 2);
		FileRead((char*)BITMAPRAM2-8, size);	// Read and decrunch frame 2
		Decrunch(BITMAPRAM2-8+size);		
	  #endif	
	}
  #else
	if (FileOpen(filename)) {		
		FileRead(bmpPalette, 4);			// 4 bytes palette
		FileRead((char*)BITMAPRAM1, 8000);	// 8000 bytes for frame 1
	  #ifdef __ATARIXL__	
		FileRead((char*)BITMAPRAM2, 8000);	// 8000 bytes for frame 2
	  #endif	
	}
  #endif
  
#elif defined __CBM__
	unsigned char i;
	unsigned int size;
	if (FileOpen(filename)) { 
	  #if defined __DECRUNCH__	
		size = FileRead((char*)(BITMAPRAM), -1);
		rom_disable();											 // Disable ROM to access $dff8-$dfff
		memcpy((char*)(BITMAPRAM-8), (char*)(BITMAPRAM), size);  // Shift before decrunching
		for (i=0; i<3; i++) {
			Decrunch(BITMAPRAM-8+PEEKW(BITMAPRAM-4-2*i));		 // Decrunch 1 block 		
			if (i==0) { 										 // COLOR RAM			
				rom_restore();
				memcpy((char*)(COLORRAM), (char*)(SCREENRAM), 1000);  // Copy SCREENRAM to COLORRAM (reason: cannot write to COLORAM when KERNAL disabled)
				rom_disable();
			}
		}
		rom_restore();
	  #else
		FileRead((char*)(BITMAPRAM), 8000);	// 8000 bytes bitmap ram
		FileRead((char*)(SCREENRAM), 1000); // 1000 bytes char ram
		FileRead((char*)(COLORRAM),  1000);	// 1000 bytes color ram
	  #endif
		FileClose();	
	}
	
#elif defined __LYNX__
	if (FileOpen(filename))		// Bitmap data is immmediately loaded on open
		if (autoRefresh) 
			UpdateDisplay();	
		
#elif defined __NES__
	if (FileOpen(filename)) {
		FileRead(rleData, -1);
		FillVram(&rleData[4]);
		set_chr_bank_0(2+fileIndex);	// Switch char set		
		memcpy(palBG, &rleData[0], 4);	// Copy palette data
		pal_bg(palBG);					// Re-assign palette
	}

#elif defined __ORIC__
	if (FileOpen(filename))
		FileRead((char*)BITMAPRAM, -1);	
#endif	
}
