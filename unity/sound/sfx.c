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
  #define DISABLE_TONE_A  (0x01)
  #define DISABLE_TONE_B  (0x02)
  #define DISABLE_TONE_C  (0x04)
  #define DISABLE_NOISE_A (0x08)
  #define DISABLE_NOISE_B (0x10)
  #define DISABLE_NOISE_C (0x20)
  #define DISABLE_ALL	  (0x3f)  
#endif

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif
	
#if defined __APPLE2__			   //  Period  
	unsigned char sfxData[6][1] = { {	 64  	},		// SFX_BLEEP
								    {    16 	},		// SFX_BUMP
								    {     0 	},		// SFX_ENGINE
								    {     0 	},		// SFX_INJURY
								    {     0 	},		// SFX_GUN
								    {     0 	} };	// SFX_SCREECH

								//  TONE A  	TONE B	    TONE C	 NOISE  MASKS   AMP A, B, C     ENV LO, HI, TYPE
	unsigned char mockBrd[14] = { 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00 }; 	// Mockingboard state

	void DisableChannels(void) {
		if (hasMocking) {
			mockBrd[7] |= (DISABLE_TONE_A | DISABLE_TONE_B | DISABLE_TONE_C);		
			SFXMocking(mockBrd);		
		}
	}
									
 	void PlaySFX(unsigned char index, unsigned char pitch, unsigned char volume, unsigned char channel) {
		// Prepare SFX data
		unsigned char *data = sfxData[index];
		unsigned char period = data[0];
		unsigned char interv = 8-pitch/32u;
		unsigned char ch;
		
		// Get channel
		switch (channel) {
		case 0:
			ch = DISABLE_TONE_A; break;
		case 1:
			ch = DISABLE_TONE_B; break;
		case 2:
			ch = DISABLE_TONE_C;
		}

		// Play on Mockingboard or Speaker?
		if (hasMocking) {
			// Mocking board sound
			mockBrd[7] &= ~ch;
			mockBrd[2*channel] = ~pitch;
			SFXMocking(mockBrd);
			if (!period) 
				return;	// Continuous sound
			while (period) { 
				period--; 
				if (!(period%interv)) { /* do nothing */ }
			}
			mockBrd[7] |= ch;		
			SFXMocking(mockBrd);	
		} else {
			// Speaker clicks
			if (!period) {
				period = (256-pitch)/16u;
				interv = period-1;
			}
			while (period) {
				period--;
				if (!(period%interv)) { POKE(0xc030,0); }
			}
		}
	}
	
#elif defined __ATARI__			//  Period  Control (Bits 7,6,5)		Low Freq.			Mid Freq.		High Freq.
								//						  0,0,0		Geiger Counter   Raging Fire   Rushing Air   Steam
								//						  0,X,1		Machine Gun   Engine Idle   Electric Motor   Power Transformer
								//						  0,1,0		  Calm Fire      Laboring Engine     Engine with a "miss"
								//						  1,0,0		Building Collapsing   Radio Interference   Waterfall
								//						  1,X,1								Pure Tones
								//						  1,1,0		   Airplane      Lawn Mower      Electric Razor
	unsigned char sfxData[6][2] = { {   24,	   	160		},		// SFX_BLEEP				
								    {    8,	   	192		},		// SFX_BUMP				
								    {  255,    	160		},		// SFX_ENGINE			
								    {    2,    	224		},		// SFX_GUN			
								    {    4,    	128		},		// SFX_INJURY		
								    {  255,	   	192	   	} };	// SFX_SCREECH			
	
	void SetupSFX(); // VBI for SFX samples	(see Atari/POKEY.s)
	extern unsigned char sampleTimer[4];
	extern unsigned char sampleFreq[4];
	extern unsigned char sampleCtrl[4];
	
	void PlaySFX(unsigned char index, unsigned char pitch, unsigned char volume, unsigned char channel) {
		// Prepare SFX data
		unsigned char *data = sfxData[index];
		
		// Set DLI data (for period control)
		sampleTimer[channel] = data[0];
		sampleFreq[channel] = ~pitch;
		sampleCtrl[channel] = data[1] | volume/16u;	
	}
	
#elif defined __CBM__			  //  Attack/Decay  Sustain/Release  Ctrl Attack  Ctrl Release  Pitch Mult.
	unsigned char sfxData[6][5] = { { 	 0x22,			0x09,			0x11,		0x10, 		   64	},		// SFX_BLEEP  (Ctrl: Triangle Wave)
								    {    0x22,			0xA8,			0x21,		0x20, 		   32  	},		// SFX_BUMP	  (Ctrl: Square Wave)
								    {    0x00,    		0xA8,			0x61,		0x00, 		   8	},		// SFX_ENGINE
								    {    0x10,    		0xA8,			0x21,		0x10, 		   8	},		// SFX_INJURY (Ctrl: Square Wave)
								    {    0x22,			0xA8,			0x21,		0x20, 		   8 	},		// SFX_GUN	  (Ctrl: Square Wave)
								    {    0x00,			0xA8,			0x61,		0x00, 		   8	} };	// SFX_SCREECH

	void PlaySFX(unsigned char index, unsigned char pitch, unsigned char volume, unsigned char channel) {
		// Prepare SFX data
		unsigned char *data = sfxData[index];
		struct __sid_voice *ch;
		unsigned int freq;
		
		// Set Frequency
		freq = data[4]*(data[4]+pitch);
		
		// Get channel
		switch (channel) {
		case 0:
			ch = &SID.v1; break;
		case 1:
			ch = &SID.v2; break;
		case 2:
			ch = &SID.v3;
		}
		
		// Assign registers
		ch->freq = freq;
		ch->ad = data[0];
		ch->sr = data[1];
		ch->ctrl = data[2];
		if (data[3]) { ch->ctrl = data[3]; }
	}
	
#elif defined __ORIC__			   // Env.  Period  
	unsigned char sfxData[6][2] = { {  6,   1000 },		// SFX_BLEEP
								    {  1,    100 },		// SFX_BUMP
								    {  0,      0 },		// SFX_ENGINE
								    {  6,     10 },		// SFX_INJURY
								    {  2,     20 },		// SFX_GUN
								    {  0,      0 } };	// SFX_SCREECH
								  
	extern unsigned char volumeLevel;

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

	void PlaySFX(unsigned char index, unsigned char pitch, unsigned char volume, unsigned char channel) {
		// Prepare SFX data
		unsigned char *data = sfxData[index];
		switch (volumeLevel) {
		case 0:
			volume = 0;
			if (data[0]) return;
			break;
		case 1:
			volume /= 2u;
			break;
		}
		if (data[0]) {
			pitch /= 4u;
		} else {
			pitch /= 8u; 
		}
		pitch++;
		
		// Set channel pitch (MUSIC Channel, Octave, Note, Volume)
		channel += 1;
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
	unsigned char sfxData[6][5] = { {     0,	 15,    7,    2,    1 },	// SFX_BLEEP
								    {     1,	  5,    7,    4,    1 },	// SFX_BUMP
								    {     2,	  0,   60,    2,    0 },	// SFX_ENGINE
								    {     1,	  2,    7,    4,    1 },	// SFX_INJURY
								    {     1,	  4,    7,    4,    1 },	// SFX_GUN
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
	void PlaySFX(unsigned char index, unsigned char pitch, unsigned char volume, unsigned char channel)
	{
		unsigned char *data = sfxData[index];
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

#elif defined __NES__			  		// Pulse  Ramp  Length  
	const unsigned char sfxData[6][3] = { { 0x90, 0x00, 0xf8 },		// SFX_BLEEP   (0b10010000, 0b00000000, 0b11111000)
									      { 0x90, 0x00, 0xf8 },		// SFX_BUMP    (0b10010000, 0b00000000, 0b11111000)
										  { 0xd0, 0x00, 0xf8 },		// SFX_ENGINE  (0b11010000, 0b00000000, 0b11111000)
										  { 0x90, 0x00, 0xf8 },		// SFX_INJURY  (0b10010000, 0b00000000, 0b11111000)
										  { 0x90, 0x00, 0xf8 },		// SFX_GUN     (0b10010000, 0b00000000, 0b11111000)
										  { 0x90, 0x00, 0xf8 } };	// SFX_SCREECH (0b10010000, 0b00000000, 0b11111000)
	
	// Pulse Ctrl:  DDLLC VVVV
	// D: Duty cycle of the pulse wave 00 = 12.5% 01 = 25% 10 = 50% 11 = 75%
	// L: Length Counter Halt: simply silences the channel when it counts down to 0. The envelope starts at a volume of 15 and decrements every time the unit is clocked, stopping at 0 if not looped.
	// C: Constant Volume
	// V: 4bit Volume
	
	// Ramp Ctrl:  EPPP NSSS
	// E : Enabled flag
	// P : Sweep Divider Period
	// N : Negate flag, inverts the sweep envelope
	// S : Shift count
	
	// Freq1 Ctrl: TTTT TTTT
	// T : Low 8 bits of the timer that controls the frequency
	
	// Freq2 Ctrl: LLLL LTTT
	// L : Length counter, if Length Counter Halt is 0, timer for note length
	// T : High 3 bits of timer that controls frequency
	
	// Centralized SFX function
	void PlaySFX(unsigned char index, unsigned char pitch, unsigned char volume, unsigned char channel)
	{
		unsigned char *data = (unsigned char*)sfxData[index];
		unsigned int addr = 0x4000 + (channel&1)*4;
		unsigned char pulse = data[0] | (volume/16u);
		POKE(addr++, pulse);		// Pulse   
		addr++; //POKE(0x4001,  data[1])	// Ramp
		POKE(addr++, ~(pitch/2u));	// Frequency
		POKE(addr++, data[2]);		// Length
	}
#endif

void InitSFX() 
{
#if defined __APPLE2__
	if (hasMocking == 255) 
		DetectMocking();
	DisableChannels();
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
	DisableChannels();
#elif defined __CBM__	
	StopMusic();
#elif defined __ATARI__
	unsigned char i;
	StopMusic();
	for (i=0; i<4; i++) {
		sampleTimer[i] = 0;
		POKE((char*)(0xD200+2*i), 0);
		POKE((char*)(0xD201+2*i), 0);
	}
#elif defined __ORIC__
	DisableChannels();
#elif defined __LYNX__
	unsigned char i;
	for (i=0; i<4; i++) {
		abcvolume(i, 0);
	}
#endif
}
