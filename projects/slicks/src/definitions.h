
// 8bit-Unity SDK
#include "unity.h"

// Debugging flags
//#define DEBUG_FPS
//#define DEBUG_NAV

// NetCode Toggle
//#define NETCODE

// Keyboard definitions
#if defined(__LYNX__)
	#define KB_START 	49
	#define KB_MUSIC 	50
	#define KB_NEXT  	51
	#define KB_CHAT 	KB_B
	#define KB_FLIP 	KB_F	
	#define KB_PAUSE 	KB_P
	#define KB_QUIT 	KB_R
#elif defined(__NES__)
	#define KB_START 	KB_EN
	#define KB_QUIT 	KB_SP	
	#define KB_NEXT 	KB_N
	#define KB_CHAT 	KB_C
	#define KB_PAUSE 	KB_P
#elif defined __CBM__
	#define KB_START 	CH_F1
	#define KB_NEXT  	CH_F3
	#define KB_CHAT 	KB_C	
	#define KB_QUIT 	KB_Q	
#else
	#define KB_START 	49
	#define KB_NEXT  	50
	#define KB_CHAT 	KB_C	
	#define KB_QUIT 	KB_Q	
#endif

// Game definitions
#define LEN_LAPS         4
#define MAX_PLAYERS      4
#define MAX_WAYPOINTS    8
#define MAX_RAMPS        3
#define LERP_THRESHOLD 128

#define MODE_LOCAL  1
#define MODE_ONLINE 2
#define MODE_INFO   3

#define STEP_WARMUP 1
#define STEP_RACE   2

// Interface definitions
#define ROW_CHAT (TXT_ROWS-2)
#define SLOT_COL1 8
#if defined __NES__
  #define SLOT_WIDTH 6
#else
  #define SLOT_WIDTH 8
#endif	

// Sprite definitions
#if defined __APPLE2__
  #define spriteFrames 64
#elif defined __ATARI__
  #define spriteFrames 18
#elif defined __ORIC__
  #define spriteFrames 17
#elif defined __CBM__
  #define spriteFrames 18
#elif defined __LYNX__
  #define spriteFrames 18
#elif defined __NES__
  #define spriteFrames 18
#endif

#if defined __ATARI__
  #define SPR2_SLOT 8
#else
  #define SPR2_SLOT 4
#endif

// Controller definitions
#if defined __APPLE2__
  #define LEN_CONTROL 9
#elif defined __ATARI__
  #define LEN_CONTROL 9
#elif defined __CBM__
  #define LEN_CONTROL 9
#elif defined __LYNX__
  #define LEN_CONTROL 9
#elif defined __NES__
  #define LEN_CONTROL 7
#elif defined __ORIC__
  #define LEN_CONTROL 9
#endif
#define NET_CONTROL (LEN_CONTROL-1)

// Network definitions
#define CL_ERROR  0
#define CL_LIST   1
#define CL_JOIN   2
#define CL_LEAVE  3
#define CL_READY  4
#define CL_FRAME  5
#define CL_EVENT  6
#define CL_TICKET 7

#define SV_ERROR  0
#define SV_LIST   1
#define SV_AUTH   2
#define SV_INFO   3
#define SV_FRAME  4
#define SV_EVENT  5
#define SV_OK     6

#define EVENT_RACE  1
#define EVENT_MAP   2
#define EVENT_LAP   3
#define EVENT_CHAT  4

#define ERR_TIMEOUT 127
#define ERR_CORRUPT 128
#define ERR_MESSAGE 129

// Vehicle structure
typedef struct {
	int x1, y1;  // Old Position * 8 (integer)
	int x2, y2;	 // New Position * 8 (integer)
	int ang1; 	 // Vehicle Angle
	int ang2;	 // Trajectory Angle
	int ang3;	 // AI Target Angle
	int vel;	 // Velocity
	int impx;	 // Impulse
	int impy;	 
    clock_t jmp; // Jump Time
	unsigned char joy;  // joystick
	unsigned char way;	// Current waypoint
	signed char lap;	// Current lap
} Vehicle;

// Waypoint structure
typedef struct {
	int x, y;		// Position
	signed char v[2][2];	// In/Out Vectors
} Waypoint;

// Ramp structure
typedef struct {
	int x[2], y[2];	// Coordinates
} Ramp;

// See slicks.s
#if defined __LYNX__
  void NextMusic(unsigned char blank);
#endif

// See game.c
void GameReset(void);
void GameInit(const char* map);
char GameLoop(void);

// See interface.c
#ifdef DEBUG_FPS
  void DrawFPS(unsigned long f);
#endif
void BackupRestoreChatRow(unsigned char mode);
void PrintBuffer(char *buffer);
void InputField(char *buffer, unsigned char len);
void PrintScores(void);
void PrintRace(void);
void PrintTimedOut(void);
void PrintLap(unsigned char i);
void GameMenu(void);

// See navigation.c
void LoadNavigation(char *filename);
void ResetLineUp(void);
void GetWaypoint(Vehicle *car);
int GetWaypointDistance(Vehicle *car);
int GetWaypointAngle(Vehicle *car);
char CheckWaypoint(Vehicle *car);
char CheckRamps(Vehicle *car);

// See network.c
void ServerConnect(void);
void ServerDisconnect(void);
void ServerInfo(void);
unsigned char ClientJoin(char game);
unsigned char ClientReady(void);
void ClientEvent(char event);
void ClientLeave(void);
unsigned char NetworkUpdate(void);

//See sfx.c
void BleepSFX(unsigned char pitch);
void BumpSFX(void);
void EngineSFX(unsigned char index, unsigned int rpm);
void JumpSFX(unsigned char index);
void ScreechSFX(unsigned char index);
void UpdateSFX(void);
