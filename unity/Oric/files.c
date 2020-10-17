/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2019/05/05
 *	
 * Copyright (c) 2019 Anthony Beaucamp.
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
 
#include <stdlib.h>
#include <string.h>

// Externals: see libsedoric.s
extern void __fastcall__ sed_loadfile(void);
extern void __fastcall__ sed_savefile(void);
extern void __fastcall__ sed_loadzp(void);
extern void __fastcall__ sed_savezp(void);

extern unsigned int sed_size;
extern const char* sed_fname;
extern void* sed_dest;
extern void* sed_end;
extern int sed_err;

unsigned char  fileNum;     
unsigned char* fileNames[16];
unsigned int   fileSizes[16];  
unsigned char* fileBuffer;

// Using Sedoric for File Management
void FileList()
{
	unsigned char j=0, k;
	
	// Assign some memory to file buffer
	fileBuffer = malloc(240);

	// Get raw file list from Sedoric buffer at $C310
	sed_savezp();	  // Backup Zero Page
	asm("jsr $04f2"); // Switch ON RAM/ROM overlay
	asm("jsr $D451"); // Execute Sedoric DIR
	memcpy((char*)fileBuffer, 0xC310, 240);
	asm("jsr $04f2"); // Switch OFF RAM/ROM overlay	
	sed_loadzp();	  // Restore Zero Page
	
	// Reformat and link to list
	fileNum = 0;
	while (fileNum<15 && fileBuffer[j]) {
		fileNames[fileNum] = &fileBuffer[j];
		fileSizes[fileNum] = *(unsigned int*)&fileBuffer[j+14];
		k = j; while (fileBuffer[k] != ' ') k++;
		fileBuffer[k++] = '.'; j += 9;
		fileBuffer[k++] = fileBuffer[j++];
		fileBuffer[k++] = fileBuffer[j++];
		fileBuffer[k++] = fileBuffer[j];
		fileBuffer[k] = 0;
		j = (++fileNum)*16;
	}
}

int FileWrite(const char* fname, void* buf, int len) 
{
    sed_fname = fname;
    sed_dest = buf;
    sed_end = (char*)sed_dest+len;
    sed_savefile();  	
    return sed_err;
}

int FileRead(const char* fname, void* buf)
{
    sed_fname = fname;
    sed_dest = buf;
    sed_loadfile();
    return sed_size;
}
