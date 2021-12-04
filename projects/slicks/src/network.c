
#include "definitions.h"

char networkReady = 0;
char chatBuffer[20];
char udpBuffer[28];

unsigned char clIndex, clVersion;
unsigned char clUser[5] = "";
unsigned char clPass[13] = "";
unsigned char* packet;
unsigned int svFPS, tckNET;
unsigned char clFrame, svFrame, svMap, svStep; 
unsigned char clName[MAX_PLAYERS][5];
unsigned char eData1, eData2;
clock_t timeRecv, timeSend;

// See slicks.c
extern unsigned char inkColors[];

// See game.c
extern unsigned char gameMap, gameStep;
extern unsigned char gameLineUp[4];
extern unsigned char lapGoal;
extern unsigned int lapBest[MAX_PLAYERS];

// See interface.c
extern unsigned char controlIndex[MAX_PLAYERS];

// See navigation.c
extern Vehicle cars[MAX_PLAYERS];

void NetworkTicket(char ticket)
{
#ifdef NETCODE
	// Code removed to prevent Cheating...
#endif
}

void ServerConnect()
{
#ifdef NETCODE
	// Code removed to prevent Cheating...
#endif
}

void ServerDisconnect()
{
#ifdef NETCODE
	// Code removed to prevent Cheating...
#endif
}

void ServerInfo()
{
#ifdef NETCODE
	// Code removed to prevent Cheating...
#endif
}

unsigned char ServerEvent()
{
    unsigned char event, ticket, i;
#ifdef NETCODE
	// Code removed to prevent Cheating...
#endif
    return event;
}
	
void ServerFrame()
{
#ifdef NETCODE
	// Code removed to prevent Cheating...
#endif
}

void ServerAuth()
{
#ifdef NETCODE
	// Code removed to prevent Cheating...
#endif
}

unsigned char ClientJoin(char game)
{
#ifdef NETCODE	
	// Code removed to prevent Cheating...
#endif
	return 1;
}
	
void ClientFrame()
{
#ifdef NETCODE	
	// Code removed to prevent Cheating...
#endif
}

unsigned char ClientReady()
{    
#ifdef NETCODE	
	// Code removed to prevent Cheating...
#endif
    return 1;
}

void ClientEvent(char event)
{
#ifdef NETCODE	
	// Code removed to prevent Cheating...
#endif
}

void ClientLeave()
{
#ifdef NETCODE	
	// Code removed to prevent Cheating...
#endif
}

unsigned char NetworkUpdate()
{
#ifdef NETCODE
	// Code removed to prevent Cheating...
#endif
	return 0;    
}
