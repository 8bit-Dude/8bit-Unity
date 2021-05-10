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
  #pragma code-name("LOWCODE")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

unsigned char maskInput = 0;

// Interactive text input function
unsigned char InputStr(unsigned char width, char *buffer, unsigned char len, unsigned char key)
{
	unsigned char i, curlen, offset, bckX = txtX;
	unsigned char c = '*';
	
	// Check current length of input
	curlen = strlen(buffer);
	if 	(curlen < width)
		offset = curlen;
	else
		offset = width;
		
	// Was a new key received?
	if (!key) {
		// Initialize input field
		PrintBlanks(width+1, 1);
		i = curlen-offset; 
		while (i < curlen) {
			if (!maskInput)
				c = buffer[i];
			PrintChr(c);
			txtX++; i++;
		}
		
	} else {
		// Process Return Key
		if (key == CH_ENTER) { 
			return 1; 
		} else
		// Process Delete Key
		if (key == CH_DEL) {
			// Is there something to delete?
			if (curlen > 0) {
				buffer[curlen-1] = 0;				
				if 	(curlen > width) {
					for (i=width-1; i>0; i--)
						CopyStr(txtX+i, txtY, txtX+i-1, txtY, 1);
					if (!maskInput)
						c = buffer[curlen-width-1];
					PrintChr(c);		
					txtX += offset;
				} else {
					txtX += offset;
					PrintChr(' ');
					txtX--;					
				}
			}
		} else {
			// Is field already full?
			if (curlen >= len)
				return 0;
		}
		
		// Process other Keys
	#if (defined __ATARI__) || (defined __ORIC__)
		if (key > 31 && key < 123) { // Atari/Oric
	#else
		if (key > 31 && key < 96)  { // Apple/C64/Lynx
	#endif
			buffer[curlen] = key;
			buffer[curlen+1] = 0;
			if (curlen >= width) {
				CopyStr(txtX, txtY, txtX+1, txtY, width-1);
				offset--;
			}
			if (!maskInput)
				c = key;
			txtX += offset;
			PrintChr(c);
			txtX++;
		}
	}

	// Show cursor
	PrintChr('_');
	txtX = bckX;
	return 0;
}
