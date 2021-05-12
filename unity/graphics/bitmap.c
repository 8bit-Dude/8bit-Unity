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

#ifdef __ATARI__
  void SetPalette() {
    POKE(0x02c8, bmpPalette[0]);
    POKE(0x02c4, bmpPalette[1]);
    POKE(0x02c5, bmpPalette[2]);
    POKE(0x02c6, bmpPalette[3]);			
  }
#endif

#ifdef __NES__
 #pragma bss-name(push, "XRAM")
  unsigned char rleData[0x300];  
 #pragma bss-name(pop)
  extern unsigned char vram_attr[64];
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
	
	// Set default palette
	SetPalette();
	
	// Switch OFF ANTIC
	POKE(559, (16+8+2));

	// Setup DLI/VBI
	StartDLI(); StartVBI();		
	
#elif defined __ORIC__
	// Switch to Hires mode
	if PEEK((char*)0xC800)
		asm("jsr $EC33");	// Atmos (ROM 1.1)
	else
		asm("jsr $E9BB");	// Oric-1 (ROM 1.0)
	memset((char*)0xBF68, 0, 120);	// Clear lower text area
	
#elif defined __LYNX__
	InitDisplay();
#endif	
}

// Switch from Text mode to Bitmap mode
void ShowBitmap()
{		
#if defined __CBM__
	SetupVIC2();	// Switch memory bank and multicolor mode
	POKE(0xD011, PEEK(0xD011)|32);	// Set bitmap mode
	
#elif defined __ATARI__
	// Setup DLIST and screen DMA
	BitmapDLIST(); bitmapVBI = 1;
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
	bitmapVBI = 0;
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
	bzero((char*)BITMAPRAM2, 8000);
	
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
	ppu_off();
	pal_bg(palBG);			// Re-assign default palette
	bzero(vram_attr, 64);	// Reset color attributes (shadow)
	vram_adr(NAMETABLE_A);	// Go to top of nametable
	vram_fill(0, 0x400);	// Fill with 0s
	ppu_on_all();
#endif
}

// Load bitmap from file
void LoadBitmap(char *filename) 
{
#if defined __ORIC__
	FileRead(filename, (void*)(BITMAPRAM));
	
#elif defined __LYNX__
	if (FileRead(filename) && autoRefresh) 
		UpdateDisplay();	

#elif defined __NES__
    unsigned char* data = FileRead(filename);
	if (data) {
		// Copy Palette/RLE data to XRAM
		memcpyBanked(palBG, &data[0], 4, 1);
		memcpyBanked(rleData, &data[4], fileSizes[fileIndex]-4, 1);
		
		// Decompress RLE to VRAM
		ppu_off();	
		vram_adr(NAMETABLE_A);
		vram_unrle(rleData);			// Decompress name-table
		set_chr_bank_0(2+fileIndex);	// Switch to char set
		pal_bg(palBG);					// Assign palette
		ppu_on_all();
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
		FileRead((char*)&size, 2);
		FileRead((char*)BITMAPRAM2-8, size);	// Read and decrunch frame 2
		Decrunch(BITMAPRAM2-8+size);		
		SetPalette();
	}
  #else
	if (FileOpen(filename)) {		
		FileRead(bmpPalette, 4);			// 4 bytes palette
		FileRead((char*)BITMAPRAM1, 8000);	// 8000 bytes for frame 1
		FileRead((char*)BITMAPRAM2, 8000);	// 8000 bytes for frame 2
		SetPalette();
	}
  #endif
  
#elif defined __CBM__
	unsigned char i;
	unsigned int size;
	FILE* fp = fopen(filename, "rb");  
  #if defined __DECRUNCH__	
	for (i=0; i<2; i++) {
		// Process 2 crunched blocks (screen, bitmap)
		fread((char*)&size, 1, 2, fp);				// Get crunch data size
		fread((char*)(0xff40), 1, 8, fp);			// Read first 8 bytes to temporary location
		fread((char*)(BITMAPRAM), 1, size-8, fp);	// Read crunched data
		rom_disable();								// Disable ROM to access $dff8-$dfff
		memcpy((char*)(BITMAPRAM-8), (char*)(0xff40), 8);  // Copy back first 8 bytes
		Decrunch(BITMAPRAM-8+size);					// Decrunch data		
		if (i==0) {	// Copy first block to screen address locations
			memcpy((char*)(SCREENRAM), (char*)(BITMAPRAM), 1000);
		}
		rom_enable();
	}
  #else
	fread((char*)(BITMAPRAM), 1, 8000, fp);	// 8000 bytes bitmap ram
	fread((char*)(SCREENRAM), 1, 1000, fp); // 1000 bytes char ram
  #endif
	fread((char*)(COLORRAM),  1, 1000, fp);	// 1000 bytes color ram
	fclose(fp);	
#endif
}
