
#include "definitions.h"

// SFX channel definitions
#if defined(__LYNX__) || defined(__NES__)
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
	#define VOLUME_ENGINE 60
#elif defined(__NES__)
	#define PITCH_BUMP 	    32
	#define VOLUME_BLEEP   192
	#define VOLUME_BUMP    192
	#define VOLUME_ENGINE   64
	#define VOLUME_SCREECH  80
#elif defined(__ORIC__)
	#define PITCH_BUMP    32
	#define VOLUME_BLEEP  15
	#define VOLUME_BUMP   15
	#define VOLUME_ENGINE 15
#else
	#define PITCH_BUMP 	    32
	#define VOLUME_BLEEP    60
	#define VOLUME_BUMP    120
	#define VOLUME_ENGINE   22
	#define VOLUME_SCREECH  30
#endif

// SFX Variables
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
	vol[index] = VOLUME_ENGINE;
}

#if defined(__CBM__) || defined(__LYNX__) || defined(__NES__)
void JumpSFX(unsigned char index)
{
	sfx[index] = SFX_ENGINE;
	frq[index] = 192;
	vol[index] = VOLUME_SCREECH;
}

void ScreechSFX(unsigned char index)
{
	sfx[index] = SFX_SCREECH;
#if defined(__CBM__) || defined(__NES__)	
	frq[index] = 160;
#else
	frq[index] = 192;
#endif
	vol[index] = VOLUME_SCREECH;
}
#endif

void UpdateSFX()
{		
	unsigned char i=0, fi, f1=0, f2=0, s1=0, s2=0, v1=0, v2=0;
	
	// Scan players looking for frequency priority
	while (i<4) {
		if (sfx[i]) {
			fi = frq[i]; 
			if (fi >= f1) {
				// Downgrade previous #1
				f2 = f1; s2 = s1; v2 = v1;
				
				// Assign new #1 priority
				f1 = fi; s1 = sfx[i]; v1 = vol[i];
			} else
			if (fi >= f2) {
				// Assign new #2 priority
				f2 = fi; s2 = sfx[i]; v2 = vol[i];
			}
			sfx[i] = 0;
		}
		i++;
	}
	
	// Play SFX as needed
	if (v1) PlaySFX(s1, f1, v1, 0);
	if (v2) PlaySFX(s2, f2, v2, 1);
}
