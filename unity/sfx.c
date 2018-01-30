
#include "unity.h"

#ifdef __APPLE2__
#pragma code-name("LC")
#endif

#if defined __ATARI__
	// Atari specific functions (see Atari/POKEY.s)
	void PlaySFX();
	extern unsigned char sampleCount;
	extern unsigned char sampleFreq;
	extern unsigned char sampleCtrl;
#elif defined __APPLE2__
	// Nothing for now
	void PlayMusic() {}
	void StopMusic() {}
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
	PlaySFX();	// Setup interrupt for samples
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
	unsigned char delay;
	POKE(0xc030,0);
	delay = (600-vel)/60; 
	while (delay) {
		(delay--);
	}
	POKE(0xc030,0);
#endif
}

void BleepLowSFX() 
{
#if defined __CBM__	
	SID.v3.freq = 10000;
	SID.v3.sr   = 0x09; // sustain, release
	SID.v3.ctrl = 0x11; // triangle wave, set attack bit
	SID.v3.ctrl = 0x10; // release attack bit
#elif defined __ATARI__
	sampleCount = 24;
	sampleFreq = 192;
	sampleCtrl = 170;
#elif defined __APPLE2__	
	unsigned int repeat = 64;
	while (repeat) {
		if (repeat%4) { POKE(0xc030,0); }
		repeat--;
	}
#endif
}

void BleepHighSFX() 
{
#if defined __CBM__	
	SID.v3.freq = 15000;
	SID.v3.sr   = 0x09; // sustain, release
	SID.v3.ctrl = 0x11; // triangle wave, set attack bit
	SID.v3.ctrl = 0x10; // release attack bit
#elif defined __ATARI__
	sampleCount = 24;
	sampleFreq = 128;
	sampleCtrl = 170;
#elif defined __APPLE2__	
	unsigned int repeat = 64;
	while (repeat) {
		if (repeat%2) { POKE(0xc030,0); }
		repeat--;
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
