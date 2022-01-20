
#define MAX_GRUBS 4
#define MAX_PROJS 1

#define STATE_PHYS 1
#define STATE_JUMP 2
#define STATE_MOVE 4
#define STATE_WEAP 8
#define STATE_LOAD 16

#define PROJ_NULL  0
#define PROJ_LOAD  1
#define PROJ_MOVE  2

#define KEYFRAME_MLEFT  0
#define KEYFRAME_JLEFT  3
#define KEYFRAME_MRIGHT 4
#define KEYFRAME_JRIGHT 7
#define KEYFRAME_DEAD   15
#define KEYFRAME_WEAP   8
#define KEYFRAME_PROJ   14

#define NUMFRAME_MOVE   3
#define NUMFRAME_JUMP   1
#define NUMFRAME_WEAP   3

#define EXPLODE YELLOW
#define HEALTH_LOW RED
#define HEALTH_MED ORANGE
#if defined __APPLE2__
	#define SKY 		LBLUE
	#define HEALTH_HIGH LGREEN
	#define GRND_OFFST	7
#elif defined __ATARI__
	#define SKY 		BLUE
	#define HEALTH_HIGH GREEN
	#define GRND_OFFST	4
#elif defined __CBM__
	#define SKY 		CYAN
	#define HEALTH_HIGH GREEN
	#define GRND_OFFST	4
#elif defined __LYNX__
	#define SKY 		LBLUE
	#define HEALTH_HIGH GREEN
	#define GRND_OFFST	7
#elif defined __NES__
	#define SKY 		BLACK
	#define HEALTH_HIGH GREEN
	#define GRND_OFFST	5
#elif defined __ORIC__
	#define SKY 		BLACK
	#define HEALTH_HIGH MGREEN
	#define GRND_OFFST	5
#endif

// Grub structure
typedef struct {
	unsigned int x, y;
	signed char zAcc;
	unsigned int wAng;
	unsigned char health, index, state;
	unsigned char keyFrame, motionFrame, numFrames;
	unsigned char weapFrame;
} Grub;

// Projectile structure
typedef struct {
	signed int x, y;
	signed char xVel, yVel;
	unsigned char index, owner, state;
} Proj;
