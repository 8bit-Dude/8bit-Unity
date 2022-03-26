/*****************************************************************
Ultimate 64/II+ Command Library
Scott Hutter, Francesco Sblendorio
Based on ultimate_dos-1.2.docx and command interface.docx
https://github.com/markusC64/1541ultimate2/tree/master/doc
Disclaimer:  Because of the nature of DOS commands, use this code
soley at your own risk.
Patches and pull requests are welcome

Modified by Anthony Beaucamp for compatiblity with 8bit-Unity
******************************************************************/

#include <time.h>
#include <string.h>
#include "ultimate.h"

static unsigned char *cmddatareg = (unsigned char *)CMD_DATA_REG;
static unsigned char *controlreg = (unsigned char *)CONTROL_REG;
static unsigned char *statusreg = (unsigned char *)STATUS_REG;
//static unsigned char *idreg = (unsigned char *)ID_REG;
static unsigned char *respdatareg = (unsigned char *)RESP_DATA_REG;
static unsigned char *statusdatareg = (unsigned char *)STATUS_DATA_REG;

char uii_data[DATA_QUEUE_SZ];
char uii_status[STATUS_QUEUE_SZ];

unsigned char uii_target = TARGET_DOS1;

void uii_reset_uiidata() 
{
	memset(uii_data, 0, DATA_QUEUE_SZ);
	memset(uii_status, 0, STATUS_QUEUE_SZ);
}

void uii_freeze(void)
{
	unsigned char cmd[] = {0x00,0x05};
	
	uii_target = TARGET_CONTROL;
	uii_sendcommand(cmd, 2);
	uii_readdata();
	uii_readstatus();
	uii_accept();
	
}

void uii_identify(void)
{
	unsigned char cmd[] = {0x00,DOS_CMD_IDENTIFY};
	
	uii_target = TARGET_DOS1;
	uii_sendcommand(cmd, 2);
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_echo(void)
{
	unsigned char cmd[] = {0x00,DOS_CMD_ECHO};
	
	uii_target = TARGET_DOS1;
	uii_sendcommand(cmd, 2);
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_sendcommand(unsigned char *bytes, int count)
{
	int x =0;
	int success = 0;
	clock_t timeout = clock()+60;
	
	bytes[0] = uii_target;
	
	while (!success)
	{
		// Wait for idle state
		while ( !(((*statusreg & 32) == 0) && ((*statusreg & 16) == 0)) )
			if (clock() > timeout) return;
		
		// Write byte by byte to data register
		while( x<count )
			*cmddatareg = bytes[x++];
		
		// Send PUSH_CMD
		*controlreg |= 0x01;
		
		// check ERROR bit.  If set, clear it via ctrl reg, and try again
		if ( *statusreg & 4 ) {
			*controlreg |= 0x08;
		} else {
			// check for cmd busy
			while ( !(*statusreg & 32) && (*statusreg & 16) ) {
				if (clock() > timeout) return;
			}
			success = 1;
		}
		
		// Escape route!
		if (clock() > timeout) return;
	}	
}

void uii_accept(void)
{
	clock_t timeout = clock()+30;

	// Acknowledge the data
	*controlreg |= 0x02;
	while ( *statusreg & 2 )
		if (clock() > timeout) return;
}

int uii_isdataavailable(void)
{
	if ( *statusreg & 128 )
		return 1;
	else
		return 0;
}

int uii_isstatusdataavailable(void)
{
	if ( *statusreg & 64 )
		return 1;
	else
		return 0;
}

void uii_abort(void)
{
	// abort the command
	*controlreg |= 0x04;
}

int uii_readdata(void) 
{
	int count = 0;
	uii_data[0] = 0;

	while (uii_isdataavailable() && count<255)
		uii_data[count++] = *respdatareg;
	uii_data[count] = 0;

	return count;
}

int uii_readstatus(void) 
{
	int count = 0;
	uii_status[0] = 0;
	
	while(uii_isstatusdataavailable() && count<255)
		uii_status[count++] = *statusdatareg;
	uii_status[count] = 0;
	
	return count;
}

void uii_getinterfacecount(void)
{
	unsigned char cmd[] = {0x00,NET_CMD_GET_INTERFACE_COUNT};
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(cmd, 0x02);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

void uii_getipaddress(void)
{
	unsigned char cmd[] = {0x00,NET_CMD_GET_IP_ADDRESS, 0x00}; // interface 0 (theres only one)
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(cmd, 0x03);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

unsigned char uii_socketopen(char* host, unsigned short port, char type)
{
	unsigned char len = strlen(host);
	uii_data[0] = 0x00;
	uii_data[1] = type;
	uii_data[2] = port & 0xff;
	uii_data[3] = (port>>8) & 0xff;
	memcpy(&uii_data[4], host, len);
	uii_data[4+len] = 0x00;
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(uii_data, 4+len+1);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();

	return uii_data[0];
}

void uii_socketclose(unsigned char socketid)
{
	unsigned char cmd[] = {0x00,NET_CMD_SOCKET_CLOSE, 0x00};
	cmd[2] = socketid;
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(cmd, 0x03);
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

int uii_socketread(unsigned char socketid, unsigned short length)
{
	unsigned char cmd[] = {0x00,NET_CMD_SOCKET_READ, 0x00, 0x00, 0x00};

	cmd[2] = socketid;
	cmd[3] = length & 0xff;
	cmd[4] = (length>>8) & 0xff;
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(cmd, 0x05);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
	
	return uii_data[0] | (uii_data[1]<<8);
}

int uii_tcplistenstart(unsigned short port)
{
	unsigned char cmd[] = {0x00,NET_CMD_TCP_LISTENER_START, 0x00, 0x00};
	cmd[2] = port & 0xff;
	cmd[3] = (port>>8) & 0xff;
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(cmd, 0x04);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
	
	return uii_data[0] | (uii_data[1]<<8);
}

int uii_tcplistenstop()
{
	unsigned char cmd[] = {0x00,NET_CMD_TCP_LISTENER_STOP};
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(cmd, 0x02);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
	
	return uii_data[0] | (uii_data[1]<<8);
}

int uii_tcpgetlistenstate()
{
	unsigned char cmd[] = {0x00,NET_CMD_GET_LISTENER_STATE};
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(cmd, 0x02);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
	
	return uii_data[0] | (uii_data[1]<<8);
}

unsigned char uii_tcpgetlistensocket()
{
	unsigned char cmd[] = {0x00,NET_CMD_GET_LISTENER_SOCKET};
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(cmd, 0x02);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
	
	return uii_data[0] | (uii_data[1]<<8);
}

void uii_socketwrite(unsigned char socketid, char *data, unsigned char length)
{
	uii_data[0] = 0x00;
	uii_data[1] = NET_CMD_SOCKET_WRITE;
	uii_data[2] = socketid;
	memcpy(&uii_data[3], data, length);	
	uii_data[3+length] = 0;
	
	uii_target = TARGET_NETWORK;
	uii_sendcommand(uii_data, 3+length);
	
	uii_readdata();
	uii_readstatus();
	uii_accept();
}

