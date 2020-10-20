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
#include <unity.h>

// Variables containing file lists
unsigned char  fileNum;     
unsigned char* fileNames[16];
unsigned int   fileSizes[16];
unsigned char* fileBuffer;     

// Using ROM for File Management
void FileList(void)
{
	unsigned char i=0, j;
	FILE* fp;

	// Assign some memory to file buffer
	fileBuffer = malloc(256);
	
	// Open directory file, and parse it
	fp = fopen("$", "rb");
	fread(fileBuffer, 1, 0x27, fp); j = 0x21;
	while (fileNum<16) {
		// Copy file size
		fileSizes[fileNum] = *(int*)&fileBuffer[j]; j = i+0x1a;
		
		// Read from file (caution: sometimes data gets shited by 1 byte)
		if (!fread(&fileBuffer[i], 1, 0x20, fp))
			break;
		if (fileBuffer[i] == 0x22)
			i++;
		
		// Add zero termination to filename
		fileNames[fileNum] = &fileBuffer[i];
		while (fileBuffer[i] != 0x22)
			i++;
		fileBuffer[i++] = 0;
		fileNum++;
	}
	fclose(fp);
}

unsigned char FileOpen(const char* fname)
{
	return 0;
}

void FileRead(void* buf, unsigned int len)
{
}
