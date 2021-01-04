
#include "definitions.h"

#if defined(__LYNX__) || defined(__ORIC__)

unsigned char channel;

void BleepSFX(unsigned char pitch)
{
#if defined __LYNX__
	channel = 3;
#elif defined __ORIC__
	channel = 1;
#endif		
	PlaySFX(SFX_BLEEP, pitch, 60, channel, 0);
}

void BumpSFX(void)
{
#if defined __LYNX__
	channel = 3;
#elif defined __ORIC__
	channel = 1;
#endif	
	PlaySFX(SFX_BUMP, 32, 120, channel, 1);
}

void EngineSFX(unsigned char car, unsigned int rpm)
{
#if defined (__LYNX__) || defined(__ORIC__)
	channel = (car%2)+2;						// Multiplex on Channels 2/3
#endif
	PlaySFX(SFX_ENGINE, (rpm*17)/80u + car*6, 22, channel, 2);
}

void JumpSFX(unsigned char car)
{
#if defined (__LYNX__) || defined(__ORIC__)
	channel = (car%2)+2;						// Multiplex on Channels 2/3
#endif
	PlaySFX(SFX_ENGINE, 192, 30, channel, 2);
}

void ScreechSFX(unsigned char car)
{
#if defined (__LYNX__) || defined(__ORIC__)
	channel = (car%2)+2;						// Multiplex on Channels 2/3
#endif
	PlaySFX(SFX_SCREECH, 192, 30, channel, 2);
}

#endif		
