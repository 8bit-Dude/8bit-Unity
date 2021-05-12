
#include "unity.h"

char healthHeader[] = { 26 , 32, 32, 32, 0 };
char armorHeader[]  = { 28 , 32, 32, 32, 0 };
char goldHeader[]   = { '$', 32, 32, 32, 0 };
char killsHeader[]  = { 29 , 32, 32, 32, 0 };

extern unsigned char health, armor, gold, kills;

void SplashScreen(void)
{	
	// Load and show banner
	InitBitmap();
	LoadBitmap("menu.img");
	ShowBitmap();
	
	// Show credit/build
	paperColor = BLACK; 
	txtX = TXT_COLS-13; txtY = TXT_ROWS-4;
	PrintBlanks(12, 3);
#if (defined __ORIC__)
	inkColor = AIC;
#else		
	inkColor = RED; 
#endif
	txtX = TXT_COLS-12; txtY = TXT_ROWS-4;
	PrintStr("8BIT-DUNGEON"); txtY++;		
	PrintStr(" TECH DEMO  "); txtY++;		
	PrintStr(" 2021/04/18 ");
	
	// Start music
#ifndef __ORIC__	
	LoadMusic("dungeon.mus");
	PlayMusic();
#endif

	// Wait until key is pressed
	while (!kbhit()) {	
	#if defined __APPLE2__
		UpdateMusic();
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif
	}	
	
	// Exit banner screen
	ClearBitmap();
	HideBitmap();
#ifndef __ORIC__	
	StopMusic();		
#endif
}

void PrintHealth(void)
{
	inkColor = RED; 
	txtY = TXT_ROWS-1;
	txtX = 0; PrintStr(healthHeader); 
	txtX = 1; PrintNum(health);
}

void PrintArmor(void)
{
	inkColor = CYAN;   
	txtY = TXT_ROWS-1;
	txtX = 5; PrintStr(armorHeader);  
	txtX = 6; PrintNum(armor);
}

void PrintGold(void)
{
	inkColor = YELLOW; 
	txtY = TXT_ROWS-1;
	txtX = TXT_COLS-9; PrintStr(goldHeader); 
	txtX = TXT_COLS-8; PrintNum(gold);
}

void PrintKills(void)
{
	inkColor = WHITE;  
	txtY = TXT_ROWS-1;
	txtX = TXT_COLS-4; PrintStr(killsHeader); 
	txtX = TXT_COLS-3; PrintNum(kills);	
}

void PrintKey(void)
{
	inkColor = YELLOW;
	txtY = TXT_ROWS-1;
	txtX = 10; PrintStr("KEY");  	
}
