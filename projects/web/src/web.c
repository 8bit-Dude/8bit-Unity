
#include "unity.h"

#define TYPE_TEXT  1
#define TYPE_IMAGE 2

// HTML content types
const char ctImg[] = "Content-Type: image/jpg\r\nCache-Control: max-age=999999, public";
const char ctTxt[] = "Content-Type: text/html";

// Hint: max string length is 255, but better to keep below 192 bytes!
const char htmlHead[] = "<html><center>Welcome to 8bit-Unity Web Server<br><br><img src=\"logo.jpg\" width=\"48\"><br><br><a href=\"support\">Supported</a> platforms | <a href=\"future\">Future</a> platforms";
const char htmlSup1[] = "<br><br><table style=\"border:1px solid black;text-align:center\"><tr><td>Apple //</td><td>Atari XL/XE</td><td>Commodore 64</td><td>Oric</td><td>Lynx</td></tr><tr><td><img src=\"apple.jpg\" width=\"64\">";
const char htmlSup2[] = "</td><td><img src=\"atari.jpg\" width=\"64\"></td><td><img src=\"c64.jpg\" width=\"64\"></td><td><img src=\"atmos.jpg\" width=\"64\"></td><td><img src=\"lynx.jpg\" width=\"64\"></td></tr></table>";
const char htmlFut1[] = "<br><br><table style=\"border:1px solid black;text-align:center\"><tr><td>BBC</td><td>NES</td><td>MSX</td><td>CPC</td><td>...</td></tr></table>";
const char htmlFoot[] = "<br></center></html>";

// Shared files (jpeg images)
const char *jpgName[6] = { "logo.jpg", "apple.jpg", "atari.jpg", "atmos.jpg", "c64.jpg", "lynx.jpg" };
unsigned char *jpgData[6];
unsigned int jpgSize[6];

int main (void)
{
	unsigned char i, state, type, ip[16];
	unsigned int counter, fraction, size;
	unsigned int packet, requests;
#if defined __ORIC__
	unsigned char buffer[0x0600];
#endif	
	
	// Load resources
	for (i=0; i<6; i++) {
	#if defined __ORIC__
		jpgSize[i] = FileRead(jpgName[i], buffer);	
		jpgData[i] = (unsigned char*)malloc(jpgSize[i]);
		memcpy(jpgData[i], (unsigned char*)buffer, jpgSize[i]);
	#elif defined __LYNX__
		jpgSize[i] = FileLoad(jpgName[i]);	
		jpgData[i] = (unsigned char*)malloc(jpgSize[i]);
		memcpy(jpgData[i], (unsigned char*)SHAREDRAM, jpgSize[i]);
	#endif	
	}
	
	// Set text mode colors
    textcolor(COLOR_WHITE);
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);	
	
	// Clear screen
	clrscr();
	
	// Display interface
	gotoxy(10, 2); cprintf("Web Server:");
	gotoxy(8, 4);  cprintf("----------------------");		
	gotoxy(0, 6);  cprintf("Last Req:");		
	gotoxy(0, 8);  cprintf("Requests served: 0");		
	
	// Init network and listen on UDP port 5000
	state = InitNetwork();
	gotoxy(22, 2);
	if (state == ADAPTOR_ERR) {
		cprintf("Adaptor ERROR");
		while (1) {}
	} else if (state == DHCP_ERR) {
		cprintf("DHCP ERROR");
		while (1) {}
	} else {
		cprintf("Started!");
		
	}

	// Show local IP
	GetLocalIP(ip);
	gotoxy(12, 3); cprintf("IP: ");
	gotoxy(16, 3); cprintf(ip);
	
	// Setup Web Server on port 80 (with 3000 ms time-out)
	OpenWEB(80, 3000);
	
	// Start serving requests
	while (1) {
		// Look for incoming connection (with 10 ms time-out)
		packet = RecvWEB(10);
		if (packet) {
			// Requesting favicon? Return logo image
			if (!strncmp(packet, "GET /favicon", 12)) 
				memcpy((char*)(packet+5), (char*)"logo.jpg", 8);
				
			// Check if image was requested...
			type = TYPE_TEXT;
			for (i=0; i<6; i++) {
				if (!strncmp((char*)(packet+5), jpgName[i], 6)) {
					type = TYPE_IMAGE;
					break;
				}
			}
			
			// Return request information
			if (type == TYPE_IMAGE) {
				HeaderWEB((char*)ctImg, strlen(ctImg));
				counter = 0;
				while (counter < jpgSize[i]) {
					fraction = jpgSize[i]-counter;
					if (fraction > 192) fraction = 192;
					BodyWEB((jpgData[i]+counter), fraction);
					counter += fraction;
				}
			} else {
				HeaderWEB((char*)ctTxt, strlen(ctTxt));
				if (!strncmp(packet, "GET / ", 6)) {				
					BodyWEB((char*)htmlHead, strlen(htmlHead));
					BodyWEB((char*)htmlFoot, strlen(htmlFoot));
				} else
				if (!strncmp(packet, "GET /support", 12)) {				
					BodyWEB((char*)htmlHead, strlen(htmlHead));
					BodyWEB((char*)htmlSup1, strlen(htmlSup1));
					BodyWEB((char*)htmlSup2, strlen(htmlSup2));
					BodyWEB((char*)htmlFoot, strlen(htmlFoot));
				} else 
				if (!strncmp(packet, "GET /future", 11)) {
					BodyWEB((char*)htmlHead, strlen(htmlHead));
					BodyWEB((char*)htmlFut1, strlen(htmlFut1));
					BodyWEB((char*)htmlFoot, strlen(htmlFoot));
				}
			}
			SendWEB();
			
			// Update stats
			gotoxy(10, 6); cprintf("                              ");
			gotoxy(10, 6); cprintf(packet);
			gotoxy(17, 8); cprintf("%u", ++requests);
		}
	}
	
	// Close connection
	CloseWEB();
    return EXIT_SUCCESS;
}
