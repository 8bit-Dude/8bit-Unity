/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2020/10/12
 *	
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
 *
 */

#include <peekpoke.h>

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#define XBIOS_BUFFER 0x0400

// Variables containing file list
unsigned char  fileNum;     
unsigned char* fileNames[24];
unsigned int   fileSizes[24];
unsigned char  fileBuffer[312]; // 24*13 bytes

// Externals: see xbios.s
extern void __fastcall__ xbios_list_dir(void);
extern unsigned char __fastcall__ xbios_get_entry(void);

// Using XBIOS for File Management
void DirList(void)
{
	unsigned char i, j=0, k;
	
	// Get file list
	xbios_list_dir();
	
	// Go through xbios buffer
	i = xbios_get_entry();
	while (fileNum<24 && PEEK(XBIOS_BUFFER+i)) {
		fileNames[fileNum] = &fileBuffer[j];
		fileSizes[fileNum] = PEEKW(XBIOS_BUFFER-4+i) * 256;
		k = 0;
		while (k<8) {
			if (PEEK(XBIOS_BUFFER+i+k) != 0x20)
				fileBuffer[j++] = PEEK(XBIOS_BUFFER+i+k);
			k++;
		}
		fileBuffer[j++] = '.';
		while (k<11) {
			fileBuffer[j++] = PEEK(XBIOS_BUFFER+i+k);
			k++;
		}
		fileBuffer[j++] = 0;
		i = xbios_get_entry();
		fileNum++;
	}
}
