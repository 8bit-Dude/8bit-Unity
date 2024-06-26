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

extern unsigned char  fileNum;     
extern unsigned char* fileNames[];

unsigned char CompareNames(const char* fname1, const char* fname2)
{
	unsigned char i=0, c1, c2;

	while (1) {
		c1 = fname1[i];
		c2 = fname2[i];
		if (!c1 && !c2) return 0;
		if (c1 > 0xC0) c1 -= 0x80; 
		if (c2 > 0xC0) c2 -= 0x80; 
		if (c1 > 0x60) c1 -= 0x20; 
		if (c2 > 0x60) c2 -= 0x20; 
		if (c1 != c2) break;
		i++;
	}
	return i;
}
 
unsigned char FileExists(const char* fname)
{
	unsigned char i=0;
	
	DirList();
	while (i<fileNum) {
		if (!CompareNames(fname, fileNames[i]))
			return 1;
		i++;
	}
	return 0;
}
