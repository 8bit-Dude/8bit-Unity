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

// Variables containing file list
unsigned char  fileNum = 0;
unsigned int   fileSizes[16];  
unsigned char* fileNames[16];

// Externals: see prodos.s
/* extern void __fastcall__ prodos_open_file(void);
extern void __fastcall__ prodos_load_data(void);

extern const char* prodos_fname;
extern void* prodos_dest;
extern unsigned int prodos_len; */

void FileList()
{
	DIR *dir;
	struct dirent *dp;
	if ((dir = opendir(".")) != NULL) {
		do {
			dp = readdir(dir);
			if (dp != NULL) {
				fileNames[fileNum] = malloc(strlen(dp->d_name));
				strcpy(fileNames[fileNum], dp->d_name);
				fileNum++;
			}
		} while (dp != NULL);
		closedir(dir);
	}
}

/* unsigned char FileOpen(const char* fname)
{
	*((char*)prodos_fname-1) = strlen(fname);
    prodos_fname = fname;
	prodos_open_file();	
	return 1;
}

void FileRead(void* buf, unsigned int len)
{
	prodos_dest = buf;
  prodos_len  = len;
  prodos_load_data();	
} */
