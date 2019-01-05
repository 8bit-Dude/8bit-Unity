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

#if defined __ATARI__
	// Atari specific functions (see Atari/POKEY.s)
	void SetupSFX(); // VBI for SFX samples
	extern unsigned char sampleCount;
	extern unsigned char sampleFreq;
	extern unsigned char sampleCtrl;
#elif defined __APPLE2__
	// Apple specific functions (see Apple/DUET.s and Apple/MOCKING.S)
	unsigned char sfxOutput = 0;
	void SFXMocking(unsigned int address);
	void PlayMocking(unsigned int address);		
	void StopMocking();		
	void PlaySpeaker(unsigned int address);
	// Wrapper functions (for speaker / mocking)
	void PlayMusic(unsigned int address) { 
		if (sfxOutput) { PlayMocking(address); } else { PlaySpeaker(address); } 
	}
	void StopMusic() { 
		if (sfxOutput) { StopMocking(); }
	}
	// Mockingboards sounds:        TONE A  	TONE B	    TONE C	 NOISE  MASKS   AMP A, B, C     ENV LO, HI, TYPE
	unsigned char sfxData[14] = { 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00 }; 
    #define DISABLE_TONE_A  (0x01)
    #define DISABLE_TONE_B  (0x02)
    #define DISABLE_TONE_C  (0x04)
    #define DISABLE_NOISE_A (0x08)
    #define DISABLE_NOISE_B (0x10)
    #define DISABLE_NOISE_C (0x20)
	#define DISABLE_ALL		(0x3f)
#endif

void InitSFX() 
{
#if defined __CBM__
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
#endif
}

void EngineSFX(int channel, int vel)
{
#if defined __CBM__	
	unsigned int freq = 4*vel+(channel*5+200);
	if (channel%2) {
		SID.v2.freq = freq;	
	} else {
		SID.v1.freq = freq;	
	}
#elif defined __ATARI__
	unsigned char freq = (200-vel/4)+channel*5;
	POKE((char*)(0xD200+2*channel), freq);
	POKE((char*)(0xD201+2*channel), 16*2+8);
#elif defined __APPLE2__	
	unsigned char tone;
	if (sfxOutput) {
		// Mocking board sound
		tone = (252-vel/4);
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
		tone = (600-vel)/60; 
		while (tone) { (tone--); }
		POKE(0xc030,0);
	}	
#endif
}

#ifdef __APPLE2__
#pragma code-name("LC")
#endif

void BleepSFX(unsigned char tone) 
{
#if defined __CBM__	
	SID.v3.freq = 78*(320-tone);
	SID.v3.sr   = 0x09; // sustain, release
	SID.v3.ctrl = 0x11; // triangle wave, set attack bit
	SID.v3.ctrl = 0x10; // release attack bit
#elif defined __ATARI__
	sampleCount = 24;
	sampleFreq = tone;
	sampleCtrl = 170;
#elif defined __APPLE2__
	unsigned char repeat = 64;
	unsigned char interval = tone/48;
	if (sfxOutput) {
		// Mocking board sound
		sfxData[7] &= ~DISABLE_TONE_C;
		sfxData[4] = tone;
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
#endif
}

void BumpSFX() 
{
#if defined __CBM__	
	SID.v3.freq = 2000;
	SID.v3.sr   = 0xA8; // sustain, release
	SID.v3.ctrl = 0x21; // square wave, set attack bit
	SID.v3.ctrl = 0x20; // release attack bit
#elif defined __ATARI__
	sampleCount = 16;
	sampleFreq = 255;
	sampleCtrl = 232;
#endif
}
