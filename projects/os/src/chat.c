
#include "definitions.h"

#if defined(__NES__)
 #pragma rodata-name("BANK0")
 #pragma code-name("BANK0")
#endif

#define CHAT_CODE

#define REQ_LOGIN 	1
#define REQ_PAGE 	2
#define REQ_RECV 	3
#define REQ_SEND 	4
#define REQ_HEADER  85
#define REQ_ERROR   170

#if defined(__LYNX__)
  #define MSG_PER_PAGE 3
#else
  #define MSG_PER_PAGE 4
#endif

// See network.c
extern char netConnected;

unsigned char lineX1, lineX2, lineY, chatLogged, chatLen;
unsigned int chatList[4] = {0, 0, 0, 0};
unsigned char chatRequest[140];
unsigned char* chatUser = &chatRequest[4];
unsigned char* chatPass = &chatRequest[15];
unsigned char* chatBuffer = &chatRequest[26];

callback *callUser, *callPass, *callLogin, *callMessage, *callSend, *callScroll;
unsigned int scrollRange[2] = {0, 1};

void ChatPage()
{
#if defined(CHAT_CODE)	
	chatRequest[0] = REQ_PAGE;
	POKEW(&chatRequest[1], scrollRange[0]);
	chatRequest[3] = MSG_PER_PAGE;
	SendTCP(chatRequest, 4);	
#endif
}


void ChatRefresh(void)
{
#if defined(CHAT_CODE)	
	unsigned int i;
	paperColor = DESK_COLOR;
	txtX = 0; txtY = 2;
	for (i=0; i<MSG_PER_PAGE; i++) {
		PrintBlanks(TXT_COLS-1, 4);
		txtY += 5;
	}
	ChatPage();
#endif
}

void ChatSend()
{
#if defined(CHAT_CODE)	
	// Make message length checks
	unsigned char len;
	len = strlen(chatBuffer);
	txtX = 10; txtY = 1;
	if (len < 8) {
		paperColor = DESK_COLOR; inkColor = BLACK;
		PrintStr("Min Length: 8 chars!"); 		
		sleep(1); PrintBlanks(20, 1);
	#ifndef __NES__	
		Line(lineX1, lineX2, lineY, lineY);
	#endif
		return;
	}
	
	// Send message to server
	chatRequest[0] = REQ_SEND;
	chatRequest[1] = strlen(chatUser);
	chatRequest[2] = strlen(chatPass);
	chatRequest[3] = strlen(chatBuffer);
	SendTCP(chatRequest, 26+strlen(chatBuffer));
	
	// Clear previous message
	paperColor = WHITE;
	txtX = 0; txtY = 0;
	PrintBlanks(TXT_COLS-4,1);
	chatBuffer[0] = 0;
	
	// Refresh messages
	scrollRange[0] = 0;
	ChatRefresh();
#endif
}

void ChatLogin()
{
#if defined(CHAT_CODE)	

#if defined __LYNX__ 
	// Save user/pass to EEPROM
	//unsigned char i = 0;
	//while (i < 22) {	
	//	lynx_eeprom_write(i, chatUser[i]); i++;
	//}
#endif
	// Send login request to server
	chatRequest[0] = REQ_LOGIN; 
	chatRequest[1] = strlen(chatUser);
	chatRequest[2] = strlen(chatPass);
	SendTCP(chatRequest, 24);	
#endif
}

void ChatMessage(unsigned char index, unsigned char* packet)
{
#if defined(CHAT_CODE)	
	unsigned char i, l, buffer[29];

	// Find message slot
	paperColor = DESK_COLOR;
	
	// Display user/date
	inkColor = WHITE;
	txtX = 0; txtY = index*5+2;
	PrintStr(&packet[3]); txtY++;
	i = 3 + 1 + strlen(&packet[3]);
	
	PrintStr(&packet[i]); txtY--;
	i = i + 1 + strlen(&packet[i]);
	
	// Display message
	inkColor = BLACK;
	txtX = TXT_COLS-29;
	l = i + strlen(&packet[i]);
	while (i < l) {
		memcpy(buffer, &packet[i], 28);
		PrintStr(buffer);
		i += 28; txtY++;
	}
#endif
}

void ChatScreen(void)
{			
#if defined(CHAT_CODE)	
	unsigned char i=0;
	
	// Clear screen
	ClearScreen();
	DrawTaskBar();		
	paperColor = DESK_COLOR; 
	inkColor = BLACK;	
	
	// Do we have net access?
	if (!netConnected) {
		Panel(10, 3, 20, 9, "");	
		txtX = 12; txtY = 7;
		PrintStr("Network Init...");	
		ServerConnect();
	}
	
	if (!netConnected) {
		PrintStr(" No internet!  ");	
		return;
	}
	
	// Login and Message screen?		
	if (!chatLogged) {
		// Panel/Labels
		Panel(10, 3, 20, 9, "");	
		txtX = 11; txtY = 5;
		PrintStr("User:"); txtY += 2;
		PrintStr("Pass:"); txtY += 2;
		PrintStr("Reg 8bit-unity.com");
		
	#if defined __LYNX__ 
		// Load user/pass from EEPROM
		//while (i < 22) { 
		//	chatUser[i] = lynx_eeprom_read(i); i++;
		//}
	#endif			
		
		// Inputs
		paperColor = WHITE;
		callUser = Input(16, 5, 10, 1, chatUser, 10);
		maskInput = 1;
		callPass = Input(16, 7, 10, 1, chatPass, 10);
		maskInput = 0;
		
		// Controls
		paperColor = BLACK; inkColor = WHITE;	
		callLogin = Button(17, 11, 7, 1, " Login ");
		
	} else {
		// Add text input, send button, and scrollbar
		paperColor = BLACK; inkColor = WHITE;	
		callSend = Button(TXT_COLS-4, 0, 7, 1, "Send");
		
		paperColor = WHITE; inkColor = BLACK;
		callMessage = Input(0, 0, TXT_COLS-5, 1, chatBuffer, 112);
			
		paperColor = DESK_COLOR;
		callScroll = ScrollBar(TXT_COLS-1, 1, TXT_ROWS-2, scrollRange);
		
		// Add separators
	#ifndef __NES__
		lineX1 = ColToX(0)+2;
		lineX2 = ColToX(TXT_COLS-2)+2;
		for (i=0; i<MSG_PER_PAGE; i++) {
			lineY = RowToY(5*i+1)+3;
			Line(lineX1, lineX2, lineY, lineY);
		}
	#endif
		// Request page
		ChatPage();		
	}	
#endif
}

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

void ChatPacket(unsigned char *packet)
{
#if defined(CHAT_CODE)	
	unsigned char i;

	// Process received packets
	switch 	(packet[0]) {
	case REQ_LOGIN:
		// Check if login was OK
		if (packet[1] == 0x6f) {  // Ascii code for 'o'
			chatLogged = 1;	
			ChatScreen();
		} else {
			paperColor = DESK_COLOR; inkColor = BLACK;
			txtX = 12; txtY = 13;
			PrintStr(&packet[1]);
		}
		break;
		
	case REQ_PAGE:
		// Save chat list and request first message
		scrollRange[1] = *(unsigned int*)&packet[4];
		chatLen = packet[6];
		for (i=0; i<chatLen; i++)
			chatList[i] = packet[7+2*i];
		chatRequest[0] = REQ_RECV;			
		POKEW(&chatRequest[1], chatList[0]); 
		chatRequest[3] = PLATFORM;		
		SendTCP(chatRequest, 3);
		break;
	
	case REQ_RECV:
		// Check corresponding entry in list
		i = 0;
		while (i<MSG_PER_PAGE) {
			if ((unsigned int)packet[1] == chatList[i]) break;
			i++;
		}

		// Display message and request next entry
		ChatMessage(i, packet);
		if (i<chatLen-1) {
			POKEW(&chatRequest[1], chatList[i+1]); 
			SendTCP(chatRequest, 3);
		}
		break;
	}
#endif
}

void ChatCallback(callback* call)
{
#if defined(CHAT_CODE)	
	if (!chatLogged) {
		if (call == callPass) {
			maskInput = 1;
		} else {
			maskInput = 0;
		}
		if (call == callLogin) {
			ChatLogin();
			return;
		}
	} else {
		if (call == callScroll) {
			ChatRefresh();
			return;
		}
		if (call == callSend) {
			ChatSend();
			return;
		}
	}
#endif
}
