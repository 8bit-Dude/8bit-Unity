/*
 * Copyright (c) 2018 Anthony Beaucamp.
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

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif	

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif	

void LoadMusic(const char* filename)
{
#if defined __ORIC__
	// Load from File
	FileRead(filename, (char*)MUSICRAM);
#elif defined __LYNX__
	// Load from CART file system
	FileRead(filename);
/* #elif defined __APPLE2__
	if (FileOpen(filename)) {
		// Consume 2 bytes of header then read data
		FileRead((char*)MUSICRAM, 2);	
		FileRead((char*)MUSICRAM, 8000);	
	} */
#elif defined __ATARI__
	if (FileOpen(filename)) {
		// Consume 6 bytes of header then read data
		FileRead((char*)MUSICRAM, 6);	
		FileRead((char*)MUSICRAM, 0x0700);	
	}
#else
	FILE* fp;
	unsigned int loadaddr;

	// Try to open file
	fp = fopen(filename, "rb");	
	if (!fp) return;

	// Get load address and read data
	fread((char*)&loadaddr, 1, 2, fp);
  #if defined __APPLE2__
	fread((char*)loadaddr, 1, 0x0300, fp);
  #elif defined __CBM__
	fread((char*)loadaddr, 1, 0x1000, fp);  
  #endif
#endif
}

#ifndef __ATARI__	
#ifndef __CBM__	
#ifndef __ORIC__	

void PlayMusic(unsigned int address) 
{
#if defined __APPLE2__
	// Wrapper functions (for speaker / mocking)
	if (hasMocking == 255)
		DetectMocking();
	if (hasMocking) 
		PlayMocking(address); 
	else 
		PlaySpeaker(address);
#elif defined __LYNX__	
	unsigned char i = 0;
	lynx_snd_pause();
	while (i<4) {
		if PEEKW(address) lynx_snd_play(i, PEEKW(address));
		address += 2;
		++i;
	}
	lynx_snd_continue();
#endif	
}

void StopMusic() 
{ 
#if defined __APPLE2__
	if (hasMocking) 
		StopMocking();
#elif defined __LYNX__	
	lynx_snd_stop();
#endif	
}

#endif
#endif
#endif
