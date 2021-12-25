/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2021/12/25
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

// Externals: see libsedoric.s
extern void __fastcall__ sed_loadfile(void);
extern void __fastcall__ sed_savefile(void);

extern unsigned int sed_size;
extern const char* sed_fname;
extern void* sed_dest;
extern void* sed_end;
extern int sed_err;

unsigned int FileOpen(const char* fname)
{
	sed_fname = fname;
	return 1;
}

unsigned int FileRead(char* buffer, signed int len)
{
    sed_dest = (void*)buffer;
    sed_loadfile();
    return sed_size;
}

void FileWrite(char* buffer, signed int len) 
{
    sed_dest = (void*)buffer;
    sed_end = (void*)((unsigned int)sed_dest+len);
    sed_savefile();  	
    //return sed_err;
}

void FileClose( void )
{
}
