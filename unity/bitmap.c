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

#if defined __CBM__  
  unsigned char bg = 0;
#endif

// Initialize Bitmap Screen
void InitBitmap() 
{
#if defined __APPLE2__
	// Prepare Double Hi-Res Mode
	asm("sta $c052"); // TURN ON FULLSCREEN       
	asm("sta $c057"); // TURN ON HI-RES           
  #if defined __DHR__
	asm("sta $c001"); // TURN ON 80 STORE
  #endif	
  
#elif defined __ATARI__
	// Disable cursor
	POKEW(0x0058, 0);	// SAVMSC
	POKEW(0x005E, 0);	// OLDADR
	POKE(0x005D, 0);	// OLDCHR
	
	// Switch OFF ANTIC
	POKE(559, 2);
		
	// Set default palette
	POKE(0x02c8, 0x00);
	POKE(0x02c4, 0x24);
	POKE(0x02c5, 0x86);
	POKE(0x02c6, 0xd8);		
	
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
void EnterBitmapMode()
{		
#if defined __CBM__
	// Setup VIC2 (memory bank and multicolor mode)
	SetupVIC2();

	// Set VIC2 to bitmap mode
	POKE(0xD011, PEEK(0xD011)|32);		
	
#elif defined __ATARI__
	// Setup DLIST
	BitmapDLIST();	
  #if defined __ATARIXL__
	// Setup frame flicker DLI (only on XL)
	bitmapDLI = 1; StartDLI();	
  #endif
	// ANTIC: DMA Screen
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
void ExitBitmapMode()
{
#if defined __CBM__
	// Switch OFF multicolor mode
	ResetVIC2();
		
#elif defined __ATARI__
    // Switch screen DMA and DLI
	POKE(559, PEEK(559)&~32);
	bitmapDLI = 0; 
	
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
void ClearBitmap()
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
		POKE((char*)BITMAPRAM+y*40, (y%2) ? 6 : 3);

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
/*#elif defined __APPLE2__
	if (FileOpen(filename)) {
		*dhraux = 0;  FileRead((char*)BITMAPRAM, 8192);	// Read 8192 bytes to AUX	
		*dhrmain = 0; FileRead((char*)BITMAPRAM, 8192); // Read 8192 bytes to MAIN
	}*/
#elif defined __ATARI__
	if (FileOpen(filename)) {		
		FileRead((char*)PALETTERAM, 4);		// 4 bytes for palette
		POKE(0x02c8, PEEK(PALETTERAM+0));
		POKE(0x02c4, PEEK(PALETTERAM+1));
		POKE(0x02c5, PEEK(PALETTERAM+2));
		POKE(0x02c6, PEEK(PALETTERAM+3));	
		FileRead((char*)BITMAPRAM1, 8000);	// 8000 bytes for frame 1
		FileRead((char*)BITMAPRAM2, 8000);	// 8000 bytes for frame 2
	}
#else
	// Open Map File
	FILE* fp = fopen(filename, "rb");	
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
  #if defined __DHR__
	*dhraux = 0;
	fread((char*)BITMAPRAM, 1, 8192, fp);
	*dhrmain = 0;
  #endif	
	// Read 8192 bytes to MAIN
	fread((char*)BITMAPRAM, 1, 8192, fp);
  #endif
	// Close file
	fclose(fp);
#endif
}
