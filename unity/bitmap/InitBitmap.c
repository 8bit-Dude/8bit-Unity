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
	chrRows = 128;		// Prevents palette switching
	StartDLI(); StartVBI();	
  #ifdef __ATARIXL__
	doubleBuffer = 1;
  #endif
  
#elif defined __LYNX__
	InitDisplay();
	  
#elif defined __ORIC__
	// Switch to Hires mode
	if PEEK((char*)0xC800)
		asm("jsr $EC33");	// Atmos (ROM 1.1)
	else
		asm("jsr $E9BB");	// Oric-1 (ROM 1.0)
	memset((char*)0xBF68, 0, 120);	// Clear lower text area
	
#endif	
}
