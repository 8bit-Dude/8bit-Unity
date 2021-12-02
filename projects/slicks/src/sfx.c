
#include "definitions.h"

// SFX channel definitions
#if defined(__LYNX__)
	#define CHAN_BLEEP 1
	#define CHAN_BUMP  1
#else
	#define CHAN_BLEEP 2
	#define CHAN_BUMP  2
#endif

// SFX variations
#if defined(__ATARI__)
	#define PITCH_BUMP    16
	#define VOLUME_BLEEP  60
	#define VOLUME_BUMP   60
	#define VOLUME_LOW 	  60
	#define VOLUME_HIGH   90
#else
	#define PITCH_BUMP 	  32
	#define VOLUME_BLEEP  60
	#define VOLUME_BUMP   120
	#define VOLUME_LOW 	  22
	#define VOLUME_HIGH   30
#endif

unsigned char sfx[4], frq[4], vol[4];

void BleepSFX(unsigned char pitch)
{
	PlaySFX(SFX_BLEEP, pitch, VOLUME_BLEEP, CHAN_BLEEP);
}

void BumpSFX(void)
{
	PlaySFX(SFX_BUMP, PITCH_BUMP, VOLUME_BUMP, CHAN_BUMP);
}

void EngineSFX(unsigned char index, unsigned int rpm)
{	
	sfx[index] = SFX_ENGINE;
	frq[index] = (rpm*17)/80u + index*6;
	vol[index] = VOLUME_LOW;
}

void JumpSFX(unsigned char index)
{
	sfx[index] = SFX_ENGINE;
	frq[index] = 192;
	vol[index] = VOLUME_HIGH;
}

void ScreechSFX(unsigned char index)
{
	sfx[index] = SFX_SCREECH;
#if defined(__CBM__)	
	frq[index] = 160;
#else
	frq[index] = 192;
#endif
	vol[index] = VOLUME_HIGH;
}

void UpdateSFX()
{		
	unsigned char i=0, f1=0, f2=0, s1=0, s2=0, v1=0, v2=0;
	
	// Scan players looking for frequency priority
	while (i<4) {
		if (sfx[i]) {
			if (frq[i] > f1) {
				// Downgrade previous #1
				f2 = f1; s2 = s1; v2 = v1;
				
				// Assign new #1 priority
				f1 = frq[i]; s1 = sfx[i]; v1 = vol[i];
			} else
			if (frq[i] > f2) {
				// Assign new #2 priority
				f2 = frq[i]; s2 = sfx[i]; v2 = vol[i];
			}
			sfx[i] = 0;
		}
		i++;
	}
	
	// Play SFX as needed
	if (f1) PlaySFX(s1, f1, v1, 0);
	if (f2) PlaySFX(s2, f2, v2, 1);
}
