
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
	#define VOLUME_ENGINE 60
	#define VOLUME_JUMP   90
#else
	#define PITCH_BUMP 	  32
	#define VOLUME_BLEEP  60
	#define VOLUME_BUMP   120
	#define VOLUME_ENGINE 22
	#define VOLUME_JUMP   30
#endif

void BleepSFX(unsigned char pitch)
{
	PlaySFX(SFX_BLEEP, pitch, VOLUME_BLEEP, CHAN_BLEEP);
}

void BumpSFX(void)
{
	PlaySFX(SFX_BUMP, PITCH_BUMP, VOLUME_BUMP, CHAN_BUMP);
}

void EngineSFX(unsigned char channel, unsigned int rpm)
{	
	PlaySFX(SFX_ENGINE, (rpm*17)/80u + channel*6, VOLUME_ENGINE, channel);	// Multiplex on channels 1/2
}

void JumpSFX(unsigned char channel)
{
	PlaySFX(SFX_ENGINE, 192, VOLUME_JUMP, channel);	// Multiplex on channels 1/2
}

void ScreechSFX(unsigned char channel)
{
	PlaySFX(SFX_SCREECH, 192, VOLUME_JUMP, channel);	// Multiplex on channels 1/2
}
