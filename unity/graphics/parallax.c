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
  
// Pointers to various data sets
#if defined __ATARI__
  extern unsigned char parallaxDLI;
#endif
extern unsigned char *screenData;
extern unsigned char blockWidth, lineWidth;

// Initialize Parallax Mode
void InitParallax() 
{
	// Define rendering window
	lineWidth = 44;
	blockWidth = 44;
	screenWidth = 44;
	
#if (defined __ATARI__) || (defined __CBM__)
	screenData = (char*)(SCREENRAM);
#elif (defined __LYNX__) || (defined __ORIC__)
	screenData = (char*)(BITMAPRAM+1);
#endif

#if (defined __APPLE2__) || (defined __LYNX__) || (defined __ORIC__)
	InitBitmap();
#elif defined __ATARI__	
	InitBitmap();
#elif defined __CBM__
#endif
}

// Switch from Text mode to Parallax mode
void ShowParallax()
{
#if (defined __APPLE2__)
	ShowBitmap();
	
#elif defined __ATARI__		
	// Set palette, DLIST and screen DMA
	SetPalette(chrPalette);	
	ParallaxDLIST(); parallaxDLI = 1;
	POKE(559, PEEK(559)|32);	
	
#elif defined __CBM__	
	SetupVIC2(); // Switch memory bank and multicolor mode	
	StartDLI();  // Setup raster line interrupt
#endif	
}

// Switch back to Text mode
void HideParallax()
{
#if defined __APPLE2__	
	HideBitmap();
	
#elif defined __ATARI__	
	// Disable Screen DMA and VBI
	POKE(559, PEEK(559)&~32);
	parallaxDLI = 0;
	
#elif defined __CBM__	
	StopDLI();	  // Stop raster line interrupt
	ResetVIC2();  // Exit multicolor mode
#endif	
}
