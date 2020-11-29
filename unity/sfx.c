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

#if defined __ATARI__
	// Atari specific functions (see Atari/POKEY.s)
	void SetupSFX(); // VBI for SFX samples
	extern unsigned char sampleCount;
	extern unsigned char sampleFreq;
	extern unsigned char sampleCtrl;
	
#elif defined __APPLE2__	
	// Mockingboards sounds:        TONE A  	TONE B	    TONE C	 NOISE  MASKS   AMP A, B, C     ENV LO, HI, TYPE
	unsigned char sfxData[14] = { 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00 }; 
    #define DISABLE_TONE_A  (0x01)
    #define DISABLE_TONE_B  (0x02)
    #define DISABLE_TONE_C  (0x04)
    #define DISABLE_NOISE_A (0x08)
    #define DISABLE_NOISE_B (0x10)
    #define DISABLE_NOISE_C (0x20)
	#define DISABLE_ALL		(0x3f)
	
#elif defined __ORIC__
	void ResetChannels(void) {
		// Play 7,0,0: enable channels 1/2/3 (with no enveloppe)
		POKEW(0x02E1, 7);
		POKEW(0x02E3, 0);
		POKEW(0x02E5, 0);
		if PEEK((char*)0xC800) {
			asm("jsr $FBD0");	// Atmos (ROM 1.1)
		} else {
			asm("jsr $F421");	// Oric-1 (ROM 1.0)
		}		
	}
	
	void PlaySFX(unsigned char tone, unsigned int enveloppe) {
		// Music 1,octave,note,0: set the tone (volume 0 allows changing the enveloppe)
		POKEW(0x02E1, 1);
		POKEW(0x02E3, tone/12u);
		POKEW(0x02E5, tone%12);
		POKEW(0x02E7, 0);
		if PEEK((char*)0xC800) {
			asm("jsr $FC18");	// Atmos (ROM 1.1)
		} else {
			asm("jsr $F424");	// Oric-1 (ROM 1.0)
		}
		
		// Play 1,0,1,enveloppe: set a decaying enveloppe for channel 1
		POKEW(0x02E1, 1);
		POKEW(0x02E3, 0);
		POKEW(0x02E5, 1);
		POKEW(0x02E7, enveloppe);
		if PEEK((char*)0xC800) {
			asm("jsr $FBD0");	// Atmos (ROM 1.1)
		} else {
			asm("jsr $F421");	// Oric-1 (ROM 1.0)
		}		
	}
	
#elif defined __LYNX__
	// ABCmusic functions (see lynx/sfx.s)
	extern unsigned char abctimers[4];	// Timers of musics channels
	extern void __fastcall__ abcoctave(unsigned char chan, unsigned char val);	  // legal values 0..6
	extern void __fastcall__ abcpitch(unsigned char chan, unsigned char val);	  // legal values 0..255
	extern void __fastcall__ abctaps(unsigned char chan, unsigned int val);		  // legal values 0..511
	extern void __fastcall__ abcintegrate(unsigned char chan, unsigned char val); // legal values 0..1
	extern void __fastcall__ abcvolume(unsigned char chan, unsigned char val);	  // legal values 0..127
#endif

void InitSFX() 
{
#if defined __APPLE2__
	if (hasMocking == 255)
		DetectMocking();
#elif defined __CBM__
	SID.flt_freq = 0xA000;
	SID.flt_ctrl = 0x44;
	SID.amp      = 0x1F;
	SID.v1.ad    = 0x00; // attack, delay
	SID.v1.sr    = 0xA8; // sustain, release
	SID.v1.ctrl  = 0x61; // pulse
	SID.v2.ad    = 0x00; // attack, delay
	SID.v2.sr    = 0xA8; // sustain, release
	SID.v2.ctrl  = 0x61; // pulse
	SID.v3.ad    = 0x22; // attack, delay	
#elif defined __ATARI__
	POKE((char*)0xD208,0);  // AUDCTL
	POKE((char*)0xD20F,3);
	SetupSFX();	// VBI for SFX samples
#elif defined __ORIC__
	ResetChannels();
#endif
}

void StopSFX() 
{
#if defined __APPLE2__
	if (hasMocking) 
		StopMocking();
#elif defined __CBM__	
	StopMusic();
#elif defined __ATARI__
	StopMusic();
	POKE((char*)(0xD200), 0);
	POKE((char*)(0xD202), 0);
	POKE((char*)(0xD204), 0);
	POKE((char*)(0xD208), 0);
#elif defined __ORIC__
	// Play 0,0,0: disable channels 1/2/3
	POKEW(0x02E1, 0);
	POKEW(0x02E3, 0);
	POKEW(0x02E5, 0);
	if PEEK((char*)0xC800) {
		asm("jsr $FBD0");	// Atmos (ROM 1.1)
	} else {
		asm("jsr $F421");	// Oric-1 (ROM 1.0)
	}
#elif defined __LYNX__
	unsigned char i;
	for (i=0; i<4; i++) {
		abcvolume(i, 0);
	}
#endif
}

void EngineSFX(unsigned int channel, unsigned int rpm)
{
#if defined __CBM__	
	unsigned int freq = rpm*3u+(channel*5u+200);
	if (channel%2)
		SID.v2.freq = freq;	
	else
		SID.v1.freq = freq;	
	
#elif defined __ATARI__
	unsigned char freq = (200-rpm/4u)+channel*5u;
	POKE((char*)(0xD200+2*channel), freq);
	POKE((char*)(0xD201+2*channel), 32+8);
	
#elif defined __APPLE2__	
	unsigned char tone;
	if (hasMocking) {
		// Mocking board sound
		tone = (252-rpm/4u);
		if (channel%2) {
			sfxData[7] &= ~(DISABLE_TONE_B);
			sfxData[2] = tone;
		} else {
			sfxData[7] &= ~(DISABLE_TONE_A);
			sfxData[0] = tone+3;
		}
		SFXMocking(sfxData);
	} else {
		// Speaker clicks
		POKE(0xc030,0);
		tone = (600-rpm)/60u; 
		while (tone) { (tone--); }
		POKE(0xc030,0);
	}	
#elif defined __ORIC__
	rpm = rpm/20u + 1;
	POKEW(0x02E1, channel%2+2);
	POKEW(0x02E3, rpm/12u);
	POKEW(0x02E5, rpm%12);
	POKEW(0x02E7, 0x09);
	if PEEK((char*)0xC800) {
		asm("jsr $FC18");	// Atmos (ROM 1.1)
	} else {
		asm("jsr $F424");	// Oric-1 (ROM 1.0)
	}
#elif defined __LYNX__
	unsigned char freq = (220-rpm/6u)+channel*5u;
	channel = (channel%2)+2;
	abctaps(channel, 60);
	abcoctave(channel, 2);
	abcvolume(channel, 20);
	abcintegrate(channel, 0);
	abcpitch(channel, freq);
#endif
}

void ScreechSFX(unsigned char channel, unsigned char pitch)
{
#if defined __LYNX__
	channel = (channel%2)+2;
	abctaps(channel, 107);
	abcoctave(channel, 2);
	abcvolume(channel, 30);
	abcintegrate(channel, 1);
	abcpitch(channel, ~pitch);
#else
	EngineSFX(channel, pitch*3);
#endif
}

void BleepSFX(unsigned char tone) 
{
#if defined __CBM__	
	SID.v3.freq = 78*(64+tone);
	SID.v3.sr   = 0x09; // sustain, release
	SID.v3.ctrl = 0x11; // triangle wave, set attack bit
	SID.v3.ctrl = 0x10; // release attack bit
	
#elif defined __ATARI__
	sampleCount = 24;
	sampleFreq = ~tone;
	sampleCtrl = 170;
	
#elif defined __APPLE2__
	unsigned char interval;
	unsigned char repeat = 64;
	interval = 8-tone/32u;
	
	if (hasMocking) {
		// Mocking board sound
		sfxData[7] &= ~DISABLE_TONE_C;
		sfxData[4] = ~tone;
		SFXMocking(sfxData);		
		while (repeat) { 
			if (repeat%interval) { }
			repeat--; 
		}
		sfxData[7] |= DISABLE_TONE_C;		
		SFXMocking(sfxData);	
	} else {
		// Speaker clicks		
		while (repeat) {
			if (repeat%interval) { POKE(0xc030,0); }
			repeat--;
		}
	}
#elif defined __ORIC__
	PlaySFX(tone/4u+12, 1000);
	ResetChannels();	
	
#elif defined __LYNX__	
	abctaps(2, 7);
	abcoctave(2, 2);
	abcvolume(2, 60);
	abcintegrate(2, 1);
	abcpitch(2, ~tone);
	abctimers[2] = 15;
#endif
}

void BumpSFX() 
{
#if defined __CBM__	
	SID.v3.freq = 2000;
	SID.v3.sr   = 0xA8; // sustain, release
	SID.v3.ctrl = 0x21; // square wave, set attack bit
	SID.v3.ctrl = 0x20; // release attack bit

#elif defined __APPLE2__
	unsigned char repeat = 8;
	while (repeat) {
		if (repeat&3) { POKE(0xc030,0); }
		repeat--;
	}
#elif defined __ATARI__
	sampleCount =  8;
	sampleFreq = 255;
	sampleCtrl = 232;

#elif defined __ORIC__
	PlaySFX(16, 100);
	ResetChannels();	

#elif defined __LYNX__	
	abctaps(1, 7);
	abcoctave(1, 4);
	abcvolume(1, 80);
	abcintegrate(1, 1);
	abcpitch(1, 192);
	abctimers[1] = 10;
#endif
}
