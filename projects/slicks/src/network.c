
#include "definitions.h"

#define NETCODE

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

// See interface.h
extern unsigned char paperBuffer;

void NetworkTicket(char ticket)
{
#ifdef NETCODE
#endif
}

void ServerConnect()
{
}

void ServerDisconnect()
{
}

void ServerInfo()
{
#ifdef NETCODE
#endif
}

unsigned char ServerEvent()
{
    // Server Event Packet    
    unsigned char event, ticket, i;
#ifdef NETCODE
#endif
    return event;
}
	
void ServerFrame()
{
#ifdef NETCODE
#endif
}

void ServerAuth()
{
#ifdef NETCODE
#endif
}

unsigned char ClientJoin(char game)
{
#ifdef NETCODE	
#endif
	return 1;
}
	
void ClientFrame()
{
#ifdef NETCODE	
#endif
}

unsigned char ClientReady()
{    
#ifdef NETCODE	
#endif
    return 1;
}

void ClientEvent(char event)
{
#ifdef NETCODE	
#endif
}

void ClientLeave()
{
#ifdef NETCODE	
#endif
}

unsigned char NetworkUpdate()
{
#ifdef NETCODE
#endif
	return 0;    
}
