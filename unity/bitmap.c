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
  #pragma code-name("SHADOW_RAM2")
#endif

// Lynx specific variables & functions
#ifdef __LYNX__   
  unsigned char videoInit = 0;	 
  static int palette[] =  { 0x01ca, 0x03b4, 0x08c4, 0x0cc3, 0x0c53, 0x0822, 0x0552, 0x0527, 
							0x075e, 0x0e0f, 0x09af, 0x034d, 0x0248, 0x0fff, 0x0888, 0x0000 };
  void __fastcall__ SuzyInit(void);
#endif

// C64 specific variables & functions
#ifdef __CBM__
  int vicconf[3];
  unsigned char bg = 0;
  void SwitchBank(char bank) {
	// notice that 0 selects vic bank 3, 1 selects vic bank 2, etc.
	POKE(0xDD00, (PEEK(0xDD00) & 252 | (3 - bank))); // switch VIC base
  }
#endif

// Initialize Bitmap Screen
void InitBitmap() 
{
#if defined __APPLE2__
	// Prepare Double Hi-Res Mode
	asm("sta $c052"); // TURN ON FULLSCREEN       
	asm("sta $c057"); // TURN ON HI-RES           
	asm("sta $c001"); // TURN ON 80 STORE
#elif defined __ATARI__
	// Set default palette
	POKE(PALETTERAM+0, 0x00);
	POKE(PALETTERAM+1, 0x24);
	POKE(PALETTERAM+2, 0x86);
	POKE(PALETTERAM+3, 0xd8);
#elif defined __ORIC__
	// Switch to Hires mode
	if PEEK((char*)0xC800) {
		asm("jsr $EC33");	// Atmos (ROM 1.1)
	} else {
		asm("jsr $E9BB");	// Oric-1 (ROM 1.0)
	}
	memset((char*)0xBF68, 0, 120);	// Clear text area
#elif defined __CBM__
	// Backup VIC config
	vicconf[0] = PEEK(53272);
	vicconf[1] = PEEK(53265);
	vicconf[2] = PEEK(53270);
#elif defined __LYNX__
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
#endif	
}

// Switch from Text mode to Bitmap mode
void EnterBitmapMode()
{		
#if defined __CBM__
	// Set data direction flag and Switch bank
	POKE(0xDD02, (PEEK(0xDD02) | 3));
	SwitchBank(VIDEOBANK);

	// Enter multicolor mode
	POKE(0xD018, SCREENLOC*16 + BITMAPLOC);	// 53272: address of screen and bitmap RAM
	POKE(0xD011, PEEK(0xD011) | 32);		// 53265: set bitmap mode
	POKE(0xD016, PEEK(0xD016) | 16);		// 53270: set multicolor mode
#elif defined __ATARI__
	// Setup DLIST	
	unsigned int addr = 0x0923;
	POKE(addr++, 0x4e);
	POKE(addr++, 0x10);
	POKE(addr++, 0x70);
	while (addr<0x098b) 
		POKE(addr++, 0x0e);
	POKE(addr++, 0x4e);
	POKE(addr++, 0x00);
	POKE(addr++, 0x80);
	while (addr<0x09ee) 
		POKE(addr++, 0x0e);
	POKE(addr++, 0x8e);
	POKE(addr++, 0x41);
	POKE(addr++, 0x20);
	POKE(addr++, 0x09);

	// Assign palette
	POKE(0x02c8, PEEK(PALETTERAM+0));
	POKE(0x02c4, PEEK(PALETTERAM+1));
	POKE(0x02c5, PEEK(PALETTERAM+2));
	POKE(0x02c6, PEEK(PALETTERAM+3));	
	
	// Setup ANTIC: DMA Screen + Enable P/M + DMA Players + DMA Missiles + Single resolution
	POKE(559, 32+16+8+4+2);
  #if defined __ATARIXL__
	// Setup frame flicker DLI (only on XL, which has enough RAM for 2 frames)
	frameFlicker = 1; SetupFlickerDLI();	
  #endif
#elif defined __APPLE2__
	// Switch ON Double Hi-Res Mode
	asm("sta $c00d"); // TURN ON 80 COLUMN MODE	  
    asm("sta $c050"); // TURN ON GRAPHICS         
    asm("sta $c05e"); // TURN ON DOUBLE HI-RES
#endif
}

// Switch from Bitmap mode to Text mode
void ExitBitmapMode()
{
#if defined __CBM__
	// Return VIC and Bank back to previous state
	POKE(53272, vicconf[0]);
	POKE(53265, vicconf[1]);
	POKE(53270, vicconf[2]);
	SwitchBank(0);
	
#elif defined __ATARI__
    // Switch OFF frame flicker and antic
	frameFlicker = 0;
	POKE(559, 2);
	
#elif defined __APPLE2__
    // Switch OFF Double Hi-Res Mode
    asm("sta $c051"); // TEXT - HIDE GRAPHICS
    asm("sta $c05f"); // TURN OFF DOUBLE RES
	asm("sta $c00c"); // TURN OFF 80 COLUMN MODE	  
#endif
}

// Clear entire bitmap screen
void ClearBitmap()
{
#if defined __APPLE2__
    // clear main and aux screen memory	
	*dhraux = 0;
    bzero((char *)BITMAPRAM, 8192);
	*dhrmain = 0;
    bzero((char *)BITMAPRAM, 8192);
#elif defined __ATARI__
	bzero((char*)BITMAPRAM1, 8000);
	bzero((char*)BITMAPRAM2, 8000);
#elif defined __ORIC__
	// reset pixels and set AIC Paper/Ink
	unsigned char y;
	memset((char*)BITMAPRAM, 64, 8000);	
    for (y=0; y<200; y++) {
		POKE((char*)BITMAPRAM+y*40, (y%2) ? 6 : 3);
	}	
#elif defined __CBM__
	bzero((char*)BITMAPRAM, 8000);
	bzero((char*)SCREENRAM, 1000);
	bzero((char*)COLORRAM,  1000);
#elif defined __LYNX__
	unsigned int i;
	memset(BITMAPRAM, 0xff, 8364); 
	for (i=0; i<102; i++) { 
		POKE((char*)BITMAPRAM+i*82,    0x52); 
		POKE((char*)BITMAPRAM+i*82+81, 0x00); 
	}
	UpdateDisplay();
#endif
}

// Load bitmap from file
void LoadBitmap(char *filename) 
{
#if defined __ORIC__
	// Load directly to bitmap ram
	FileRead(filename, (void*)(BITMAPRAM));
#elif defined __LYNX__
	// Load from CART file system
	if (FileRead(filename) && autoRefresh) 
		UpdateDisplay();	
/* #elif defined __APPLE2__
	if (FileOpen(filename)) {
		*dhraux = 0;  FileRead((char*)BITMAPRAM, 8192);	// Read 8192 bytes to AUX	
		*dhrmain = 0; FileRead((char*)BITMAPRAM, 8192); // Read 8192 bytes to MAIN
	} */
#elif defined __ATARI__
	if (FileOpen(filename)) {
		FileRead((char*)PALETTERAM, 4);		// 4 bytes for palette
		FileRead((char*)BITMAPRAM1, 8000);	// 8000 bytes for frame 1
		FileRead((char*)BITMAPRAM2, 8000);	// 8000 bytes for frame 2
	}
#else
	// Open Map File
	FILE* fp;
	fp = fopen(filename, "rb");	
  #if defined __CBM__
	// Consume two bytes of header
	fgetc(fp); fgetc(fp);
	
	// 8000 bytes bitmap ram
	fread((char*)(BITMAPRAM), 1, 8000, fp);

	// 1000 bytes char ram
	fread((char*)(SCREENRAM), 1, 1000, fp);
	
	// 1000 bytes color ram
	fread((char*)(COLORRAM), 1, 1000, fp);
	
	// 1 byte background color
	bg = (char) fgetc(fp);
  #elif defined __APPLE2__
	// Read 8192 bytes to AUX
	*dhraux = 0;
	fread((char*)BITMAPRAM, 1, 8192, fp);
	
	// Read 8192 bytes to MAIN
	*dhrmain = 0;
	fread((char*)BITMAPRAM, 1, 8192, fp);
  #endif
	// Close file
	fclose(fp);
#endif
}
