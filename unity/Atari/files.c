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
#include <stdlib.h>

#define XBIOS_BUFFER 0x0400

// Variables containing file lists
unsigned char  fileNum;     
unsigned char* fileNames[16];
unsigned int   fileSizes[16];
unsigned char* fileBuffer;     

// Externals: see libsedoric.s
extern void __fastcall__ xbios_list_dir(void);
extern unsigned char __fastcall__ xbios_get_entry(void);
extern unsigned char __fastcall__ xbios_open_file(void);
extern void __fastcall__ xbios_load_data(void);

extern const char* xbios_fname;
extern void* xbios_dest;
extern unsigned int xbios_len;

// Using Sedoric for File Management
void FileList(void)
{
	unsigned char i, j=0, k;

	// Assign some memory to file buffer
	fileBuffer = malloc(256);
	
	// Get file list
	xbios_list_dir();
	
	// Go through xbios buffer
	i = xbios_get_entry();
	while (PEEK(XBIOS_BUFFER+i)) {
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

unsigned char FileOpen(const char* fname)
{
	// Convert filename to XBIOS format
	unsigned char i=0, j=0, rename[11];
	while (i<8) {
		if (fname[j] != '.')
			if (fname[j] > 96)
				rename[i++] = fname[j++]-32;
			else
				rename[i++] = fname[j++];
		else
			rename[i++] = ' ';
	}
	j++;
	while (i<11) {
		if (fname[j] > 96)
			rename[i++] = fname[j++]-32;
		else
			rename[i++] = fname[j++];
	}
    xbios_fname = rename;
	return xbios_open_file();
}

void FileRead(void* buf, unsigned int len)
{
	xbios_dest = buf;
    xbios_len  = len;
    xbios_load_data();	
}
