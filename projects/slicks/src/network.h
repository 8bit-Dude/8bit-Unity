
#include "navigation.h"

#define NETCODE

#define CL_VER  8

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

char networkReady = 0;
char chatBuffer[20];
char udpBuffer[28];

unsigned char clIndex;
unsigned char clUser[5] = "";
unsigned char clPass[13] = "";
unsigned int svFPS, tckNET, packet;
unsigned char clFrame, svFrame, svMap, svStep; 
unsigned char svUsers[MAX_PLAYERS][5];
unsigned char eData1, eData2;
clock_t timeRecv, timeSend;

// Print functions (see interface.h)
void PrintLap(unsigned char i);
void PrintRace(void);
extern unsigned char paperBuffer;

void NetworkTicket(char ticket)
{
	// Code has been removed (to prevent hacking)
}

void ServerInfo()
{
	// Code has been removed (to prevent hacking)
}

unsigned char ServerEvent()
{
	// Code has been removed (to prevent hacking)
    return 0;
}

void ServerFrame()
{
	// Code has been removed (to prevent hacking)
}

void ServerAuth()
{
	// Code has been removed (to prevent hacking)
}

unsigned char ClientJoin(char game)
{
	// Code has been removed (to prevent hacking)
	return ERR_TIMEOUT;
}
	
void ClientFrame()
{
	// Code has been removed (to prevent hacking)
}

unsigned char ClientReady()
{    
	// Code has been removed (to prevent hacking)
    return ERR_TIMEOUT;
}

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

void ClientEvent(char event)
{
	// Code has been removed (to prevent hacking)
}

void ClientLeave()
{
	// Code has been removed (to prevent hacking)
}

unsigned char NetworkUpdate()
{
	// Code has been removed (to prevent hacking)
	return 0;    
}
