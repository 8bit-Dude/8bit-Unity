
#include "definitions.h"

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
	chatRequest[0] = REQ_PAGE;
	POKEW(&chatRequest[1], scrollRange[0]);
	chatRequest[3] = MSG_PER_PAGE;
	SendTCP(chatRequest, 4);	
}


void ChatRefresh(void)
{
	unsigned int i;
	paperColor = DESK_COLOR;
	for (i=0; i<MSG_PER_PAGE; i++)
		PrintBlanks(0, 5*i+2, CHR_COLS-1, 4);
	ChatPage();
}

void ChatSend()
{
	// Make message length checks
	unsigned char len;
	len = strlen(chatBuffer);
	if (len < 8) {
		paperColor = DESK_COLOR; inkColor = BLACK;
		PrintStr(10,1,"Min Length: 8 chars!"); 		
		sleep(1); PrintBlanks(10,1, 20, 1);
		Line(lineX1, lineX2, lineY, lineY);
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
	PrintBlanks(0,0,CHR_COLS-4,1);
	chatBuffer[0] = 0;
	
	// Refresh messages
	scrollRange[0] = 0;
	ChatRefresh();
}

void ChatLogin()
{
	chatRequest[0] = REQ_LOGIN; 
	chatRequest[1] = strlen(chatUser);
	chatRequest[2] = strlen(chatPass);
	SendTCP(chatRequest, 24);	
}

void ChatMessage(unsigned char index, unsigned char* packet)
{
	unsigned char i, l, line, buffer[29];

	// Find message slot
	paperColor = DESK_COLOR;
	line = index*5+2;
	
	// Display user/date
	inkColor = WHITE;
	PrintStr(0, line, &packet[3]);
	i = 3 + 1 + strlen(&packet[3]);
	
	PrintStr(0, line+1, &packet[i]);
	i = i + 1 + strlen(&packet[i]);
	
	// Display message
	inkColor = BLACK;
	l = i + strlen(&packet[i]);
	while (i < l) {
		memcpy(buffer, &packet[i], 28);
		PrintStr(CHR_COLS-29, line, buffer);
		i += 28; line++;
	}
}

void ChatScreen(void)
{			
	unsigned char i;
	
	// Clear screen
	ClearScreen();
	DrawTaskBar();		
	paperColor = DESK_COLOR; 
	inkColor = BLACK;	
	
	// Do we have net access?
	if (!netConnected) {
		Panel(10, 3, 20, 9, "");	
		PrintStr(12, 7, "Network Init...");	
		ServerConnect();
	}
	
	if (!netConnected) {
		PrintStr(12, 7, " No internet!  ");	
		return;
	}
	
	// Login and Message screen?		
	if (!chatLogged) {
		// Panel/Labels
		Panel(10, 3, 20, 9, "");	
		PrintStr(11, 5, "User:");
		PrintStr(11, 7, "Pass:");
		PrintStr(11, 9, "Reg 8bit-unity.com");
		
		// Inputs
		paperColor = WHITE;
		callUser = Input(16, 5, 10, 1, chatUser, 10);
		callPass = Input(16, 7, 10, 1, chatPass, 10);		
		
		// Controls
		paperColor = BLACK; inkColor = WHITE;	
		callLogin = Button(17, 11, 7, 1, " Login ");
		
	} else {
		// Add text input, send button, and scrollbar
		paperColor = BLACK; inkColor = WHITE;	
		callSend = Button(CHR_COLS-4, 0, 7, 1, "Send");
		
		paperColor = WHITE; inkColor = BLACK;
		callMessage = Input(0, 0, CHR_COLS-5, 1, chatBuffer, 112);
			
		paperColor = DESK_COLOR;
		callScroll = ScrollBar(CHR_COLS-1, 1, CHR_ROWS-2, scrollRange);
		
		// Add separators
		lineX1 = ColToX(0)+2;
		lineX2 = ColToX(CHR_COLS-2)+2;
		for (i=0; i<MSG_PER_PAGE; i++) {
			lineY = RowToY(5*i+1)+3;
			Line(lineX1, lineX2, lineY, lineY);
		}
		
		// Request page
		ChatPage();		
	}	
}

#ifdef __ATARIXL__
  #pragma code-name("SHADOW_RAM")
#endif

void ChatPacket(unsigned char *packet)
{
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
			PrintStr(12, 13, &packet[1]);
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
}

void ChatCallback(callback* call)
{
	if (!chatLogged) {
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
}
