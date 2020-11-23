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
  unsigned char musicPaused;	// dummy
#endif	

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif	

#ifdef __LYNX__
  unsigned char musicPaused;	// dummy
#endif	

char* musicAddr;

void LoadMusic(const char* filename, char* addr)
{
#if defined __ORIC__
	// Load from File
	FileRead(filename, addr);
#elif defined __LYNX__
	// Load from CART file system
	FileRead(filename);
/* #elif defined __APPLE2__
	if (FileOpen(filename)) {
		// Consume 2 bytes of header then read data
		FileRead(addr, 2);	
		FileRead(addr, -1);	
	} */
#elif defined __ATARI__
	unsigned int end, pos = MUSICRAM;
	if (FileOpen(filename)) {
		// Consume 6 bytes of header
		FileRead((char*)0x9000, 6);
		
		// Read up-to filesize
		end = PEEKW(0x9004);
		while (pos < end) {
			FileRead((char*)0x9000, 0x0100);	
			memcpy((char*)pos, 0x9000, 0x0100);
			pos += 0x0100;
		}
	}
#else
	// Try to open file
	unsigned int loadaddr;
	FILE* fp = fopen(filename, "rb");	
	if (fp) {
		// Consume 2 bytes of header then read data
		fread((char*)&loadaddr, 1, 2, fp);
		fread((char*)loadaddr, 1, -1, fp);
	}
#endif
	musicAddr = addr;
}

#ifndef __ATARI__	
#ifndef __CBM__	
#ifndef __ORIC__	

void PlayMusic() 
{
#if defined __APPLE2__
	// Wrapper functions (for speaker / mocking)
	if (hasMocking == 255)
		DetectMocking();
	if (hasMocking) 
		PlayMocking(musicAddr); 
	else 
		PlaySpeaker(musicAddr);
#elif defined __LYNX__	
	unsigned char i = 0;
	char* addr = musicAddr;
	lynx_snd_pause();
	while (i<4) {
		if PEEKW(addr) 
			lynx_snd_play(i, PEEKW(addr));
		addr += 2; i++;
	}
	lynx_snd_continue();
#endif	
}

#if defined __APPLE2__
void UpdateMusic() 
{
	if (hasMocking) 
		UpdateMocking(); 
	else 
		UpdateSpeaker(); 
}
#endif	

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
