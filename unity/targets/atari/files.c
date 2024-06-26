/*
 *	API of the "8bit-Unity" SDK for CC65
 *	All functions are cross-platform for the Apple IIe, Atari XL/XE, and C64/C128
 *	
 *	Last modified: 2021/12/25
 *
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

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

// Externals: see xbios.s
extern unsigned char __fastcall__ xbios_open_file(void);
extern void __fastcall__ xbios_load_data(void);

extern char* xbios_fname;
extern void* xbios_dest;
extern unsigned int xbios_len;

void FileSet(const char* fname) {
	// Convert filename to xbios format (FILENAMEEXT)
	unsigned char i=0, j=0;
	while (i<8) {
		if (fname[j] != '.')
			if (fname[j] > 96)
				xbios_fname[i++] = fname[j++]-32;
			else
				xbios_fname[i++] = fname[j++];
		else
			xbios_fname[i++] = ' ';
	}
	j++;
	while (i<11) {
		if (fname[j] > 96)
			xbios_fname[i++] = fname[j++]-32;
		else
			xbios_fname[i++] = fname[j++];
	}
}
	
// Using XBIOS for File Management
unsigned int FileOpen(const char* fname)
{
	FileSet(fname);
	return xbios_open_file();
}

signed int FileRead(char* buffer, signed int len)
{
	xbios_dest = buffer;
    xbios_len  = len;
    xbios_load_data();	
	return len;
}

/*
signed int FileWrite(char* buffer, signed int len)
{
}
*/

void FileClose( void )
{
}
