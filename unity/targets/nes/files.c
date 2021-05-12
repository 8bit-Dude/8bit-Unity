/*
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
 */

#include "../../unity.h"

#pragma rodata-name("BANK0")
#pragma code-name("BANK0")

extern unsigned char  fileNum;     	// Variable auto-generated by Build Script
extern unsigned int   fileSizes[];  //					"
extern unsigned char* fileNames[];  //					"

unsigned char fileIndex;

void DirList(void)
{
	// Do nothing, file list is assembled at build time!
}

unsigned char* FileRead(const char* filename)
{
	unsigned int addr = 0x8000;	// Base address of PRG0~PRG6 Banks
	for (fileIndex=0; fileIndex<fileNum; fileIndex++) {
		if (!strcmp(filename, fileNames[fileIndex]))
			return (unsigned char*)(addr);
		addr += fileSizes[fileIndex];
	}
	return 0;
}
