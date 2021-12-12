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

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

#ifdef __CBM__
  // see C64/SID.s
  extern unsigned int sidInitAddr;	// Defelmask default: $0903
  extern unsigned int sidPlayAddr;	// Defelmask default: $0806
#endif	

#if defined(__ATARI__) || defined(__CBM__) || defined(__ORIC__)
  extern unsigned char musicPaused;
#endif

void LoadMusic(const char* filename)
{
#if defined __ORIC__
	// Load from File
	unsigned char weird;
	FileRead(filename, (char*)MUSICRAM);
	weird = 0;	// without this code, Oric gets jammed!
#elif defined __LYNX__
	// Load from CART file system
	FileRead(filename);
#elif defined __ATARIXL__
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
#elif defined __CBM__
	// Try to open file
	unsigned int loadaddr;
	FILE* fp = fopen(filename, "rb");	
	if (fp) {
		// Consume 124 bytes of header, copy load/init/play addresses, and load SID program
		fread((char*)MUSICRAM, 1, 124, fp);
		POKE((char*)(&loadaddr)+0, PEEK((char*)(MUSICRAM+9)));
		POKE((char*)(&loadaddr)+1, PEEK((char*)(MUSICRAM+8)));
		POKE((char*)(&sidInitAddr)+0, PEEK((char*)(MUSICRAM+11)));
		POKE((char*)(&sidInitAddr)+1, PEEK((char*)(MUSICRAM+10)));
		POKE((char*)(&sidPlayAddr)+0, PEEK((char*)(MUSICRAM+13)));
		POKE((char*)(&sidPlayAddr)+1, PEEK((char*)(MUSICRAM+12)));
		if (!loadaddr) { fread((char*)(&loadaddr), 1, 2, fp); }	// Load address provided just before SID
		fread((char*)(loadaddr), 1, -1, fp);
		fclose(fp);
	}
#elif defined __APPLE2__
	if (FileOpen(filename)) {
		// Consume 2 bytes of header then read data
		FileRead((char*)MUSICRAM, 2);	
		FileRead((char*)MUSICRAM, -1);	
		FileClose();
	}	
#endif
}

void PauseMusic(unsigned char state)
{
#if defined(__ATARI__) || defined(__CBM__) || defined(__ORIC__)
	musicPaused = state;
#elif defined(__LYNX__)
	if (state)
		lynx_snd_pause();
	else
		lynx_snd_continue();
#elif defined __NES__	
	music_pause(state);	
#endif	
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
		PlayMocking((char*)MUSICRAM); 
	else 
		PlaySpeaker((char*)MUSICRAM);
#elif defined __LYNX__	
	unsigned char i = 0;
	char* addr = (char*)MUSICRAM;
	lynx_snd_pause();
	while (i<4) {
		if PEEKW(addr) 
			lynx_snd_play(i, PEEKW(addr));
		addr += 2; i++;
	}
	lynx_snd_continue();
#elif defined __NES__	
	music_play(0);
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
#elif defined __NES__	
	music_stop();
#endif	
}

#endif
#endif
#endif
