
#include "definitions.h"

// SFX channel definitions
#if defined(__LYNX__)
	#define CHAN_BLEEP 1
	#define CHAN_BUMP  1
#else
	#define CHAN_BLEEP 2
	#define CHAN_BUMP  2
#endif

void BleepSFX(unsigned char pitch)
{
	PlaySFX(SFX_BLEEP, pitch, 60, CHAN_BLEEP);
}

void BumpSFX(void)
{
	PlaySFX(SFX_BUMP, 32, 120, CHAN_BUMP);
}

void EngineSFX(unsigned char car, unsigned int rpm)
{	
	PlaySFX(SFX_ENGINE, (rpm*17)/80u + car*6, 22, car%2);	// Multiplex on channels 1/2
}

void JumpSFX(unsigned char car)
{
	PlaySFX(SFX_ENGINE, 192, 30, car%2);	// Multiplex on channels 1/2
}

void ScreechSFX(unsigned char car)
{
	PlaySFX(SFX_SCREECH, 192, 30, car%2);	// Multiplex on channels 1/2
}
