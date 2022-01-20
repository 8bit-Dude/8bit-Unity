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
#include "bitmap.h"

// Switch from Text mode to Bitmap mode
void ShowBitmap()
{		
#if defined __APPLE2__
	// Switch ON Bitmap Mode
    asm("sta $c050"); // TURN ON GRAPHICS         
  #if defined __DHR__
	asm("sta $c00d"); // TURN ON 80 COLUMN MODE	  
    asm("sta $c05e"); // TURN ON DOUBLE HI-RES
  #endif
  
#elif defined __ATARI__
	// Set palette, DLIST and screen DMA
	SetPalette(bmpPalette);	
	BitmapDLIST(); 
  #ifdef __ATARIXL__
	bitmapVBI = 1;
  #endif
	POKE(559, PEEK(559)|32);
	
#elif defined __CBM__
	SetupVIC2();	// Switch memory bank and multicolor mode
	POKE(0xD011, PEEK(0xD011)|32);	// Set bitmap mode
#endif  
}
