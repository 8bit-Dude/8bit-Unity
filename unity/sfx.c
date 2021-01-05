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

#ifdef __APPLE2__
  #pragma code-name("LC")
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif
	
#if defined __APPLE2__			//  TONE A  	TONE B	    TONE C	 NOISE  MASKS   AMP A, B, C     ENV LO, HI, TYPE
	unsigned char sfxData[14] = { 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00 }; 	// Mockingboard
    #define DISABLE_TONE_A  (0x01)
    #define DISABLE_TONE_B  (0x02)
    #define DISABLE_TONE_C  (0x04)
    #define DISABLE_NOISE_A (0x08)
    #define DISABLE_NOISE_B (0x10)
    #define DISABLE_NOISE_C (0x20)
	#define DISABLE_ALL		(0x3f)
	
#elif defined __ATARI__
	void SetupSFX(); // VBI for SFX samples	(see Atari/POKEY.s)
	extern unsigned char sampleCount;
	extern unsigned char sampleFreq;
	extern unsigned char sampleCtrl;
	
#elif defined __CBM__			  //  Attack/Decay  Sustain/Release  Ctrl Attack  Ctrl Release
	unsigned char sfxData[4][4] = { { 	  0x22,			0x09,			0x11,		0x10 	},		// SFX_BLEEP  (Ctrl: Triangle Wave)
									{     0x22,			0xA8,			0x21,		0x20 	},		// SFX_BUMP	  (Ctrl: Square Wave)
									{     0x00,    		0xA8,			0x61,		0x00 	},		// SFX_ENGINE
									{     0x00,			0xA8,			0x61,		0x00 	} };	// SFX_SCREECH
	
	void PlaySFX(unsigned char type, unsigned char pitch, unsigned char volume, unsigned char channel) {
		// Prepare SFX data
		unsigned char *data = sfxData[type];
		struct __sid_voice *ch;
		unsigned int freq;
		if (data[0]) {
			freq = 64*(32+pitch);
		} else {
			freq = 16*(8+pitch);
		}		
		
		// Get channel
		switch (channel) {
		case 0:
			ch = &SID.v1;
			break;
		case 1:
			ch = &SID.v2;
			break;
		case 2:
			ch = &SID.v3;
			break;
		}
		
		// Assign registers
		ch->freq = freq;
		ch->ad = data[0];
		ch->sr = data[1];
		ch->ctrl = data[2];
		if (data[3]) { ch->ctrl = data[3]; }
	}
	
	
#elif defined __ORIC__			  //  Env.  Period  
	unsigned char sfxData[4][2] = { {   1,    1000 },	// SFX_BLEEP
									{   1,     100 },	// SFX_BUMP
									{   0,       0 },	// SFX_ENGINE
									{   0,       0 } };	// SFX_SCREECH

	void EnableChannels(void) {
		// Play 7,0,0: enable channels 1/2/3
		POKEW(0x02E1, 7);
		POKEW(0x02E3, 0);
		POKEW(0x02E5, 0);
		if PEEK((char*)0xC800) {
			asm("jsr $FBD0");	// Atmos (ROM 1.1)
		} else {
			asm("jsr $F421");	// Oric-1 (ROM 1.0)
		}		
	}
	
	void DisableChannels(void) {
		// PLAY 0,0,0: disable channels 1/2/3
		POKEW(0x02E1, 0);
		POKEW(0x02E3, 0);
		POKEW(0x02E5, 0);
		if PEEK((char*)0xC800) {
			asm("jsr $FBD0");	// Atmos (ROM 1.1)
		} else {
			asm("jsr $F421");	// Oric-1 (ROM 1.0)
		}
	}

	void PlaySFX(unsigned char type, unsigned char pitch, unsigned char volume, unsigned char channel) {
		// Prepare SFX data
		unsigned char *data = sfxData[type];
		if (data[0]) {
			pitch = pitch/4u;
		} else {
			pitch = pitch/8u; volume /= 2;
		}
		
		// Set channel pitch (MUSIC Channel, Octave, Note, Volume)
		POKEW(0x02E1, channel);
		POKEW(0x02E3, pitch/12u+1);	// Octave
		POKEW(0x02E5, pitch%12);	// Note
		POKEW(0x02E7, data[0]?0:volume);	// (Volume 0 allows changing the enveloppe)
		if PEEK((char*)0xC800) {
			asm("jsr $FC18");	// Atmos (ROM 1.1)
		} else {
			asm("jsr $F424");	// Oric-1 (ROM 1.0)
		}
		if (!data[0]) return;
	
		// TODO: Alternative pitch setting mode (SOUND Channel, Period, Volume)
		//POKEW(0x02E1, channel);
		//POKEW(0x02E3, pitch);
		//POKEW(0x02E5, volume);
		//if PEEK((char*)0xC800) {
		//	asm("jsr $FB40");	// Atmos (ROM 1.1)
		//} else {
		//	asm("jsr $F41E");	// Oric-1 (ROM 1.0)
		//}
		
		// Set enveloppe (PLAY Tone, Noise, Envelope, Period)
		POKEW(0x02E1, channel);	// Tone:  1: Channel 1 ON -- 2: Channel 2 ON -- 4: Channel 3 ON
		POKEW(0x02E3, 0);		// Noise: 1: Channel 1 ON -- 2: Channel 2 ON -- 4: Channel 3 ON
		POKEW(0x02E5, data[0]);	// Enveloppe:  1-2: Finite, 3-7: Continuous
		POKEW(0x02E7, data[1]);	// Period
		if PEEK((char*)0xC800) {
			asm("jsr $FBD0");	// Atmos (ROM 1.1)
		} else {
			asm("jsr $F421");	// Oric-1 (ROM 1.0)
		}	

		// Re-enable all channels
		EnableChannels();
	}
	
#elif defined __LYNX__			    //  Prior  Period  Taps  Octv  Int.  
	unsigned char sfxData[4][5] = { {     0,	 15,    7,    2,    1 },	// SFX_BLEEP
									{     1,	  5,    7,    4,    1 },	// SFX_BUMP
									{     2,	  0,   60,    2,    0 },	// SFX_ENGINE
									{     2,	  0,  107,    2,    1 } };	// SFX_SCREECH
	
	// ABCmusic functions (see lynx/sfx.s)
	unsigned char abcpriority[4] = {0, 0, 0, 0};
	extern unsigned char abctimers[4];	// Timers of musics channels
	extern void __fastcall__ abcoctave(unsigned char chan, unsigned char val);	  // legal values 0..6
	extern void __fastcall__ abcpitch(unsigned char chan, unsigned char val);	  // legal values 0..255
	extern void __fastcall__ abctaps(unsigned char chan, unsigned int val);		  // legal values 0..511
	extern void __fastcall__ abcintegrate(unsigned char chan, unsigned char val); // legal values 0..1
	extern void __fastcall__ abcvolume(unsigned char chan, unsigned char val);	  // legal values 0..127
	
	// Centralized SFX function
	void PlaySFX(unsigned char type, unsigned char pitch, unsigned char volume, unsigned char channel)
	{
		unsigned char *data = sfxData[type];
		if (data[0] <= abcpriority[channel] || !abctimers[channel]) {
			abcpriority[channel] = data[0];
			abctimers[channel] = data[1];
			abctaps(channel, data[2]);
			abcoctave(channel, data[3]);
			abcintegrate(channel, data[4]);
			abcpitch(channel, ~pitch);		
			abcvolume(channel, volume);
		}
	}	
#endif

void InitSFX() 
{
#if defined __APPLE2__
	DetectMocking();
#elif defined __CBM__
	SID.flt_freq = 0xA000;	// filter frequency
	SID.flt_ctrl = 0x44;	// filter control
	SID.amp      = 0x1F;	// amplitude
#elif defined __ATARI__
	POKE((char*)0xD208,0);  // reset AUDCTL
	POKE((char*)0xD20F,3);
	SetupSFX();	// VBI for SFX samples
#elif defined __ORIC__
	EnableChannels();
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
	DisableChannels();
#elif defined __LYNX__
	unsigned char i;
	for (i=0; i<4; i++) {
		abcvolume(i, 0);
	}
#endif
}
	
#if defined(__APPLE2__) || defined(__ATARI__)
void EngineSFX(unsigned int channel, unsigned int rpm)
{
#if defined __ATARI__
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
#endif
}

void BleepSFX(unsigned char tone) 
{
#if defined __ATARI__
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
#endif
}

void BumpSFX() 
{
#if defined __APPLE2__
	unsigned char repeat = 8;
	while (repeat) {
		if (repeat&3) { POKE(0xc030,0); }
		repeat--;
	}
#elif defined __ATARI__
	sampleCount =  8;
	sampleFreq = 255;
	sampleCtrl = 232;
#endif
}
#endif
