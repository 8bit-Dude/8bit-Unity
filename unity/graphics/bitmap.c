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
#endif

#ifdef __ATARIXL__
  extern unsigned char doubleBuffer;  
#endif

#ifdef __NES__
 #pragma bss-name(push, "XRAM")
  unsigned char rleData[BITMAPRAM];  
 #pragma bss-name(pop)
  extern unsigned char fileIndex;
#endif

// Initialize Bitmap Screen
void InitBitmap() 
{
#if defined __APPLE2__
	// Prepare Double Hi-Res Mode
	asm("sta $c052"); 	// TURN ON FULLSCREEN       
	asm("sta $c057"); 	// TURN ON HI-RES           
  #if defined __DHR__
	asm("sta $c001"); 	// TURN ON 80 STORE
  #endif	
  
#elif defined __ATARI__
	// Disable cursor
	POKEW(0x0058, 0);	// SAVMSC
	POKEW(0x005E, 0);	// OLDADR
	POKE(0x005D, 0);	// OLDCHR
	
	// Switch OFF ANTIC
	POKE(559, (16+8+2));

	// Setup DLI/VBI
	StartDLI(); StartVBI();	
  #ifdef __ATARIXL__
	doubleBuffer = 1;
  #endif
  
#elif defined __ORIC__
	// Switch to Hires mode
	if PEEK((char*)0xC800)
		asm("jsr $EC33");	// Atmos (ROM 1.1)
	else
		asm("jsr $E9BB");	// Oric-1 (ROM 1.0)
	memset((char*)0xBF68, 0, 120);	// Clear lower text area
	
#elif defined __LYNX__
	InitDisplay();
	
#elif defined __NES__
	pal_bg(palBG);	// Assign default palette
#endif	
}

// Switch from Text mode to Bitmap mode
void ShowBitmap()
{		
#if defined __CBM__
	SetupVIC2();	// Switch memory bank and multicolor mode
	POKE(0xD011, PEEK(0xD011)|32);	// Set bitmap mode
	
#elif defined __ATARI__
	// Set palette, DLIST and screen DMA
	SetPalette(bmpPalette);	
	BitmapDLIST(); 
  #ifdef __ATARIXL__
	bitmapVBI = 1;
  #endif
	POKE(559, PEEK(559)|32);
	
#elif defined __APPLE2__
	// Switch ON Bitmap Mode
    asm("sta $c050"); // TURN ON GRAPHICS         
  #if defined __DHR__
	asm("sta $c00d"); // TURN ON 80 COLUMN MODE	  
    asm("sta $c05e"); // TURN ON DOUBLE HI-RES
  #endif
#endif  
}

// Switch from Bitmap mode to Text mode
void HideBitmap()
{
#if defined __CBM__
	// Switch OFF multicolor mode
	ResetVIC2();
		
#elif defined __ATARI__
    // Switch off VBI and screen DMA
  #ifdef __ATARIXL__
	bitmapVBI = 0;
  #endif
	POKE(559, PEEK(559)&~32);
	
#elif defined __APPLE2__
    // Switch OFF Double Hi-Res Mode
    asm("sta $c051"); // TEXT - HIDE GRAPHICS
  #if defined __DHR__
    asm("sta $c05f"); // TURN OFF DOUBLE RES
	asm("sta $c00c"); // TURN OFF 80 COLUMN MODE	  
  #endif	
#endif
}

// Clear entire bitmap screen
void ClearBitmap(void)
{
#if defined __APPLE2__
    // clear main and aux screen memory	
  #if defined __DHR__	
	*dhraux = 0;
    bzero((char *)BITMAPRAM, 8192);
	*dhrmain = 0;
  #endif
    bzero((char *)BITMAPRAM, 8192);
	
#elif defined __ATARI__
	// clear both frames
	bzero((char*)BITMAPRAM1, 8000);
  #ifdef __ATARIXL__
	bzero((char*)BITMAPRAM2, 8000);
  #endif
	
#elif defined __ORIC__
	// reset pixels and set AIC Paper/Ink
	unsigned char y;
	memset((char*)BITMAPRAM, 64, 8000);	
    for (y=0; y<200; y++)
		POKE((char*)BITMAPRAM+y*40, (y&1) ? 6 : 3);

#elif defined __CBM__
	bzero((char*)BITMAPRAM, 8000);
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM,  1000);
	
#elif defined __LYNX__
	unsigned char i;
	unsigned int addr = BITMAPRAM;
	memset((char*)addr, 0xff, 8364); 
	for (i=0; i<102; i++) { 
		POKE((char*)addr, 0x52); addr+=81;
		POKE((char*)addr, 0x00); addr+=1;
	}
	UpdateDisplay();

#elif defined __NES__
    FillVram(0);
#endif
}

// Load bitmap from file
void LoadBitmap(char *filename) 
{
#if defined __ORIC__
	if (FileOpen(filename))
		FileRead((char*)BITMAPRAM, -1);
	
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
	
#elif defined __APPLE2__
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
		for (i=0; i<2; i++) {
			// Process 2 crunched blocks (screen, bitmap)
			FileRead((char*)&size, 2);				// Get crunch data size
			FileRead((char*)(0xff40), 8);			// Read first 8 bytes to temporary location
			FileRead((char*)(BITMAPRAM), size-8);	// Read crunched data
			rom_disable();							// Disable ROM to access $dff8-$dfff
			memcpy((char*)(BITMAPRAM-8), (char*)(0xff40), 8);  // Copy back first 8 bytes (Trick!)
			Decrunch(BITMAPRAM-8+size);				// Decrunch data		
			if (i==0) 	// Copy first block to screen address locations
				memcpy((char*)(SCREENRAM), (char*)(BITMAPRAM), 1000);
			rom_enable();
		}
	  #else
		FileRead((char*)(BITMAPRAM), 8000);	// 8000 bytes bitmap ram
		FileRead((char*)(SCREENRAM), 1000); // 1000 bytes char ram
	  #endif
		FileRead((char*)(COLORRAM),  1000);	// 1000 bytes color ram
		FileClose();	
	}
#endif	
}
