
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

unsigned char lineX1, lineX2, lineY, chatConnected, chatLogged, chatLen;
unsigned int chatList[4] = {0, 0, 0, 0};
unsigned char chatRequest[138];
unsigned char* chatUser = &chatRequest[4];
unsigned char* chatPass = &chatRequest[14];
unsigned char* chatBuffer = &chatRequest[24];

callback *callUser, *callPass, *callLogin, *callMessage, *callSend, *callScroll;

void ChatPage(unsigned int page)
{
	chatRequest[0] = REQ_PAGE;
	POKEW(&chatRequest[1], page);
	chatRequest[3] = MSG_PER_PAGE;
	SendTCP(chatRequest, 4);	
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
	SendTCP(chatRequest, 24+strlen(chatBuffer));
	
	// Clear previous message
	paperColor = WHITE;
	PrintBlanks(0,0,36,1);
	chatBuffer[0] = 0;
	
	// Refresh messages
	ChatPage(0);	
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
	PrintStr(0, line, &packet[4]);
	i = 4 + 1 + strlen(&packet[4]);
	
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

	// DEBUG
	strcpy(chatUser, "8BIT-DUDE");
	strcpy(chatPass, "UNI-Z33K0");	
	
	// Clear screen
	ClearScreen();
	
	// Setup connection to chat server?
	if (!chatConnected) {
		OpenTCP(199, 47, 196, 106, 1999);
		chatConnected = 1;
	}
	
	// Login and Message screen?		
	if (!chatLogged) {
		// Panel/Labels
		paperColor = DESK_COLOR; inkColor = BLACK;	
		Panel(10, 3, 19, 8, "");	
		PrintStr(12, 5, "User:");
		PrintStr(12, 7, "Pass:");
		
		// Inputs
		paperColor = WHITE;
		callUser = Input(17, 5, 10, 1, chatUser);
		callPass = Input(17, 7, 10, 1, chatPass);		
		
		// Controls
		paperColor = BLACK; inkColor = WHITE;	
		callLogin = Button(17, 10, 7, 1, " Login ");
		
	} else {
		// Add text input, send button, and scrollbar
		paperColor = BLACK; inkColor = WHITE;	
		callSend = Button(CHR_COLS-4, 0, 7, 1, "Send");
		
		paperColor = WHITE; inkColor = BLACK;	
		callMessage = Input(0, 0, CHR_COLS-4, 1, chatBuffer);
			
		paperColor = DESK_COLOR;
		callScroll = ScrollBar(CHR_COLS-1, 1, CHR_ROWS-2, 0, 128);

		// Add separators
		lineX1 = ColToX(0)+2;
		lineX2 = ColToX(CHR_COLS-2)+2;
		for (i=0; i<MSG_PER_PAGE; i++) {
			lineY = RowToY(5*i+1)+3;
			Line(lineX1, lineX2, lineY, lineY);
		}
		
		// Get latest page
		ChatPage(0);		
	}
	
	// Add Taskbar
	DrawTaskBar();		
}

#ifdef __ATARI__
  #pragma code-name("SHADOW_RAM2")
#endif

void ChatPacket(unsigned char *packet)
{
	// Process received packets
	unsigned char i;
	switch 	(packet[0]) {
	case REQ_LOGIN:
		// Check if login was OK
		if (packet[1] == 'O') {
			chatLogged = 1;	
			ChatScreen();
		} else {
			PrintStr(10, 10, packet[1]);
		}
	case REQ_PAGE:
		// Save chat list and request first message
		chatLen = packet[4];
		for (i=0; i<chatLen; i++)
			chatList[i] = packet[5+2*i];
		chatRequest[0] = REQ_RECV;			
		chatRequest[1] = chatList[0]; 
		chatRequest[2] = 0; 
		chatRequest[3] = PLATFORM;		
		SendTCP(chatRequest, 4);
		break;
	
	case REQ_RECV:
		// Check corresponding entry in list
		i = 0;
		while (i<4) {
			if ((unsigned int)packet[1] == chatList[i]) break;
			i++;
		}

		// Display message and request next entry
		ChatMessage(i, packet);
		if (i<chatLen-1) {
			chatRequest[1] = chatList[i+1]; 
			SendTCP(chatRequest, 4);
		}
		break;
	}
}

void ChatCallback(callback* call)
{
	if (call == callLogin) {
		ChatLogin();
		return;
	}
	
	if (call == callSend) {
		ChatSend();
		return;
	}
}
