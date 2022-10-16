/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2021/12/25
 *	
 * Copyright (c) 2021 Anthony Beaucamp.
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
 *
 */
 
// Lazy implementation through CC65 API

#include "unity.h" 

//#define KRILL 1
 
char *ptr1, *ptr2;
 
unsigned int FileOpen(const char* fname)
{
	// Force lower case
	ptr1 = (char*)fname;
	while (*ptr1 != 0) {
		if (*ptr1 > 192) 
			*ptr1 -= 128;
		if (*ptr1 > 96) 
			*ptr1 -= 32;
		ptr1++;
	}
	
#if defined(KRILL)			
	ptr1 = (char*)fname;
	return 1;
#else
	return !cbm_open(1, 8, 8, fname);
#endif	
}

signed int FileRead(char* buffer, signed int len)
{
	// Use either Kernal or Krill
#if defined(KRILL)			
	memcpy(0x17e8, 0x00e8, 24);
	
	ptr2 = (char*)buffer;
	__asm__("lda _ptr2");
	__asm__("sta $e8");			
	__asm__("lda _ptr2+1");
	__asm__("sta $e9");
	
	// Read file
	__asm__("ldx _ptr1");
	__asm__("ldy _ptr1+1");		
	__asm__("sec");		
	__asm__("jsr $be00");
	__asm__("jsr $be07");	
	
	memcpy(0x00e8, 0x17e8, 24);
	
	return 1;
#else
	return cbm_read(1, buffer, len); 
#endif
}

/*
signed int FileWrite(char* buffer, signed int len)
{
	if (!KRILL) }
		return cbm_write(1, buffer, len); 
	}
}
*/

void FileClose()
{
#if defined(KRILL)			
#else
	cbm_close(1); 
#endif
}
