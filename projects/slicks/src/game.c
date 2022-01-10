
#include "definitions.h"

#ifdef __NES__
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")
#endif

// Navigation mask
#define TERRAIN_ROAD 0	
#define TERRAIN_DIRT 1
#define TERRAIN_WALL 2

#if defined __APPLE2__
  #define LIGHT_X  128
  #define LIGHT_Y   20
  #define LIGHT_SP  32
#elif defined __ORIC__
  #define LIGHT_X  118
  #define LIGHT_Y   24
  #define LIGHT_SP  36
#else
  #define LIGHT_X  132
  #define LIGHT_Y   24
  #define LIGHT_SP  28
#endif

// Map boundaries
#define XMIN   3*8
#define YMIN  11*8
#define XMAX 317*8
#define YMAX 189*8

// See slicks.c
extern unsigned char inkColors[];

// See interface.c
extern const char *mapList[];
extern unsigned char controlIndex[], controlBackup[], lapNumber[];

// See navigation.c
extern Vehicle cars[MAX_PLAYERS];
extern unsigned char numWays;

// See network.c
extern unsigned char clIndex;
extern unsigned char clName[MAX_PLAYERS][5];
extern unsigned char svMap, svStep; 
extern char chatBuffer[20];

// Game data
unsigned char gameMap = 0;
unsigned char gameMode = MODE_LOCAL;
unsigned char gameStep = STEP_WARMUP;
unsigned char gameLineUp[4] = { 0, 1, 2, 3 };

// Lap information
unsigned char lapIndex = 0;
unsigned char lapGoal;

// Physics parameters
int tck4, accRate, decRate, jmpTCK;
#if defined __ORIC__
  char rotRate = 2;
#else
  char rotRate = 3;
#endif
#define VELMIN   200
#define VELDRIFT 450
#define VELRAMP  800
const int velMax[4] = { 390, 460, 530, 600 };
const char rotMax[4] = { 4, 5, 6, 2 };

// Car properties and position (shared with navigation.c)
Vehicle *iCar, *jCar;	
int iX, iY;				

// Fast tables for cos/sin
const signed char cos[16] = {16,14,11,6,0,-6,-11,-14,-16,-14,-11,-6,0,6,11,14};
const signed char sin[16] = {0,6,11,14,16,14,11,6,0,-6,-11,-14,-16,-14,-11,-6};

// Clock management
clock_t gameClock;
clock_t lapClock[MAX_PLAYERS];
unsigned int lapBest[MAX_PLAYERS];
unsigned long gameFrame;

// Pause menu
#if defined(__LYNX__) || defined(__NES__)
  void BackupRestorePauseBg(unsigned char);
  unsigned char MenuPause(void);
  extern unsigned char pauseEvt;
  extern unsigned char gamePaused;
  unsigned char LockBackButton = 0;
#endif

unsigned char PlayerAvailable(unsigned char i)
{
	// CPU players not available during warmup phase
	if (controlIndex[i] > 3 || (controlIndex[i] && (gameStep != STEP_WARMUP)))
		return 1;
	return 0;
}

// Reset Game
void GameReset()
{
    unsigned char i,f;
    
    // Reset Game State
	ResetLineUp();
	
    // Reset Players
	txtY = LAPS_ROW; txtX = SLOT_COL1 + SLOT_OFFST;
    for (i=0; i<MAX_PLAYERS; ++i) {        
        // Reset SFX
        EngineSFX(i, 0);
		
		// Reset Lap count
		PrintStr("  ");
		txtX += SLOT_WIDTH;
	
		// Reset sprite
		if (PlayerAvailable(i)) {
			iCar = &cars[i];
			f = ((iCar->ang1+12)%(360))/23u;
		#if defined __APPLE2__
			f += i*16u;
			spriteX = (iCar->x2*7u)/128u;
			spriteY = (iCar->y2*3u)/25u;
		#elif defined __ATARI__
			spriteX = iCar->x2/16u + 45; 
			spriteY = iCar->y2/8u;		
		#elif defined __ORIC__
			spriteX = iCar->x2/32u;	
			spriteY = iCar->y2/8u;			
		#elif defined __CBM__
			spriteX = iCar->x2/8u; 
			spriteY = iCar->y2/8u;
		#elif defined __LYNX__
			spriteX = iCar->x2/16u; 
			spriteY = iCar->y2/16u;
		#elif defined __NES__
			spriteX =  iCar->x2/10u; 
			spriteY = (iCar->y2*3u)/25u+24;
		#endif
		#if defined MULTICOLOR
			SetMultiColorSprite(2*i, f);  // Car body and tires
			EnableMultiColorSprite(2*i);
		#else  
			SetSprite(i, f);
			EnableSprite(i);
		#endif		
		}
    }
	
	// Process sound effects
	UpdateSFX();
    
	// Display warmup message
	inkColor = WHITE; 
	if (gameStep == STEP_WARMUP) {
	#if defined __NES__
		PrintBuffer(" WARMUP: SELEC=MENU, START=RACE ");   
	#else	
        if (gameMode == MODE_ONLINE) {
		#if defined __CBM__
            PrintBuffer("  WARMUP: F1=RACE, F3=NEXT MAP, C=CHAT  ");   
		#elif defined __LYNX__
            PrintBuffer("   WARMUP: OP1=RACE, PAU=MENU, B=CHAT   ");   
		#else
            PrintBuffer("   WARMUP: 1=RACE, 2=NEXT MAP, C=CHAT   ");   
		#endif
        } else {
		#if defined __CBM__
            PrintBuffer("  WARMUP: F1=RACE, F3=NEXT MAP, Q=Quit  ");   
		#elif defined __LYNX__
            PrintBuffer("  WARMUP: OP1=RACE, PAU=MENU, RS=Quit   ");   
		#else
            PrintBuffer("   WARMUP: 1=RACE, 2=NEXT MAP, Q=Quit   ");   
		#endif
        }
	#endif
	} else {
		PrintRace();
	}
}

// Initialize Game
void GameInit(const char* map)
{
	char i, len, buffer[13];
	
	// Assign clock dependent physics params
	tck4 = 4*TCK_PER_SEC;
	accRate = (50*8)/TCK_PER_SEC;
	decRate = (100*8)/TCK_PER_SEC;
	jmpTCK = (30*TCK_PER_SEC)/50u;
	
#if defined __ATARI__
	// Adjust rotation rate on PAL systems
	if (TCK_PER_SEC < 60) { rotRate = 4; }
#endif

	// Copy Filename into Buffer
	len = strlen(map);
	memcpy(&buffer[0], map, len);
	memcpy(&buffer[len], ".img", 5);

	// Check Map is on this Disk...
#if defined(__ATARI__) || defined(__APPLE2__)
	while (!FileOpen(&buffer[0])) {
		ClearBitmap();
		txtX = 2; txtY = 12;
		inkColor = BLACK; paperColor = GREEN;
		PrintStr(" Please flip disk and press any key ");
		inkColor = WHITE; paperColor = BLACK;
		ShowBitmap(); cgetc(); HideBitmap();
	}
  #if defined(__APPLE2__)
	FileClose();	
  #endif
#endif

	// Load Bitmap and backup chat row
	LoadBitmap(&buffer[0]);
	BackupRestoreChatRow(0);
	
	// Print map name in lower left corner
	inkColor = WHITE;
	txtX = 0; txtY = LAPS_ROW;
#if defined __ORIC__
	SetAttributes(inkColor);
#endif	
	PrintStr(mapList[gameMap]);

	// Load Navigation
	memcpy(&buffer[len], ".nav", 4);
	LoadNavigation(&buffer[0]);
		
    // Some extra logics depending on the game mode
	if (gameMode == MODE_LOCAL) {
        // Setup players
		txtY = LAPS_ROW;
        for (i=0; i<MAX_PLAYERS; ++i) {
            // Print player numbers in score board
            if (controlIndex[i]) {
                inkColor = inkColors[i]; 
				txtX = SLOT_COL1 + SLOT_WIDTH*i;
			#if defined __ORIC__
				SetAttributes(inkColor);
			#endif					
				PrintChr('p'); txtX++;
				PrintChr(CHR_DIGIT+1+i);
            }
        }
        // Initiate laps
		lapGoal = lapNumber[lapIndex];
	} else {
        // Disable all players and reset user names...
        for (i=0; i<MAX_PLAYERS; ++i) { 
			clName[i][0] = 0;
            controlIndex[i] = 0;
        }
		// Assign 1st Controller to Client
		if (controlBackup[0] > 3) 
			controlIndex[clIndex] = controlBackup[0];
		else 
			controlIndex[clIndex] = 4;		
    }
}

// Game step
unsigned char GameRace()
{
    // Reset Game
	unsigned char i,res;
	unsigned int maxTime;
    GameReset();

    // Show light sprites
    for (i=SPR2_SLOT; i<SPR2_SLOT+3; ++i) {
		LocateSprite(LIGHT_X+LIGHT_SP*(i-SPR2_SLOT), LIGHT_Y);
	#if defined __ATARI__ 
		RecolorSprite(i, 0, SPR_LGREY);
	#elif defined __CBM__
		RecolorSprite(i, 0, LGREY);
	#elif defined __LYNX__
		RecolorSprite(i, 7, 0xef);
	#elif defined __ORIC__ 
		RecolorSprite(i, 0, SPR_AIC);
	#elif defined __NES__
		RecolorSprite(i, 0, 3); // Use Palette 3 (Yellow)
	#endif		
	#if defined __APPLE2__
		SetSprite(i, 64);
	#else
		SetSprite(i, 16);
	#endif		
        EnableSprite(i);
    }
  #if defined(__LYNX__)
	UpdateDisplay();
  #endif			

	// Platform dependent delay
#ifndef __APPLE2__
	gameClock = clock();
  #if defined __CBM__    
	while ((clock()-gameClock) < 25) {}
  #elif defined __ATARI__    
	while ((clock()-gameClock) < 11) {}
  #elif defined __LYNX__    
	while ((clock()-gameClock) < 35) {}
  #elif defined __ORIC__    
	while ((clock()-gameClock) < 25) {}
  #endif	
#endif	

    // Red light
#if defined __ATARI__  
	RecolorSprite(SPR2_SLOT, 0, SPR_RED);  
#elif defined __CBM__  
	RecolorSprite(SPR2_SLOT, 0, RED);  
#elif defined __LYNX__
	RecolorSprite(SPR2_SLOT, 7, 0xbf);
	UpdateDisplay();
#elif defined __ORIC__ 
	LocateSprite(LIGHT_X, LIGHT_Y);	
	RecolorSprite(SPR2_SLOT, 0, SPR_RED);
	SetSprite(SPR2_SLOT, 16);
#elif defined __NES__
	LocateSprite(LIGHT_X, LIGHT_Y);	
	RecolorSprite(SPR2_SLOT, 0, 1); // Use Palette 1 (Red)
	SetSprite(SPR2_SLOT, 16);
#elif defined __APPLE2__
	LocateSprite(LIGHT_X, LIGHT_Y);	
	SetSprite(SPR2_SLOT, 65);
#endif	
	BleepSFX(64); 
#ifndef __APPLE2__
    sleep(1);
#else
	wait(30);
#endif	

    // Orange light	
#if defined __ATARI__  
	RecolorSprite(SPR2_SLOT+1, 0, SPR_ORANGE);  
#elif defined __CBM__  
	RecolorSprite(SPR2_SLOT+1, 0, ORANGE);  
#elif defined __LYNX__
	RecolorSprite(SPR2_SLOT+1, 7, 0x8f);	
	UpdateDisplay();
#elif defined __ORIC__ 
	LocateSprite(LIGHT_X+LIGHT_SP, LIGHT_Y);	
	RecolorSprite(SPR2_SLOT+1, 0, SPR_RED);
	SetSprite(5, 16);	
#elif defined __NES__
	LocateSprite(LIGHT_X+LIGHT_SP, LIGHT_Y);	
	RecolorSprite(SPR2_SLOT+1, 0, 1); // Use Palette 1 (Red)
	SetSprite(SPR2_SLOT+1, 16);
#elif defined __APPLE2__
	LocateSprite(LIGHT_X+LIGHT_SP, LIGHT_Y);	
	SetSprite(SPR2_SLOT+1, 66);
#endif	
	BleepSFX(64); 
#ifndef __APPLE2__
    sleep(1);
#else
	wait(30);
#endif	

    // Green light
#if defined __ATARI__  
	RecolorSprite(SPR2_SLOT+2, 0, SPR_GREEN);  
#elif defined __CBM__  
	RecolorSprite(SPR2_SLOT+2, 0, GREEN);  
#elif defined __LYNX__
	RecolorSprite(SPR2_SLOT+2, 7, 0x4f);	
	UpdateDisplay();
#elif defined __ORIC__ 
	LocateSprite(LIGHT_X+2*LIGHT_SP, LIGHT_Y);	
	RecolorSprite(SPR2_SLOT+2, 0, SPR_GREEN);
	SetSprite(SPR2_SLOT+2, 16);	
#elif defined __NES__
	LocateSprite(LIGHT_X+2*LIGHT_SP, LIGHT_Y);	
	RecolorSprite(SPR2_SLOT+2, 0, 2); // Use Palette 2 (Green)	
	SetSprite(SPR2_SLOT+2, 16);	
#elif defined __APPLE2__
	LocateSprite(LIGHT_X+2*LIGHT_SP, LIGHT_Y);	
	SetSprite(SPR2_SLOT+2, 67);
#endif
	BleepSFX(128); 

	// In net mode, use client ready lag time!
	if (gameMode == MODE_ONLINE) {
		res = ClientReady(); 
        if (res == ERR_TIMEOUT) { 
			PrintTimedOut();
			return ERR_TIMEOUT; 
		} else if (res == EVENT_MAP) { 
			return EVENT_MAP;
		}
    } else {
	#ifndef __APPLE2__
		sleep(1);
	#else
		wait(30);
	#endif	
	}
	
    // Hide light sprites
    for (i=SPR2_SLOT; i<SPR2_SLOT+3; ++i) {
	#if defined __ATARI__  
		RecolorSprite(i, 0, SPR_BLACK);  
	#elif defined __CBM__  
		RecolorSprite(i, 0, BLACK);  
	#elif defined __LYNX__
		RecolorSprite(i, 7, 0xef);
		UpdateDisplay();
	#endif			
		DisableSprite(i);
	}

	// Reset game clock
	gameClock = clock();
#if defined __APPLE2__
	clk += 1;
#endif	

	// Set Max　Timer value
	if (gameMode == MODE_LOCAL)
		maxTime = LAPMAX;
	else
		maxTime = 9999;

	// Set Lap Timers
	for (i=0; i<MAX_PLAYERS; ++i) {
		lapClock[i] = gameClock;
		lapBest[i] = maxTime;
	}
	
	return 1;
}

// Angle LERPing
int deltaAngle;
int LerpAngle(int iAng1, int iAng2, int dAng)
{
	// LERP trajectory
	signed char iSign = 1;
	deltaAngle = iAng2 - iAng1;
	if (deltaAngle < 0) {
		deltaAngle = -deltaAngle;
		iSign = -1;
	}
	if (deltaAngle > 180) {
		deltaAngle = 360-deltaAngle;
		iSign = -iSign;							
	}
	if (deltaAngle > 90+dAng) {
		deltaAngle = 90+dAng;
		return iAng2 - iSign*90;		
	}
	if (deltaAngle > dAng) {
		return iAng1 + iSign*dAng;
	} else {
		return iAng2;
	}
}

char GameLoop()
{		
	// Game Management
#if (defined __ATARI__) || (defined __C64__) || (defined __LYNX__) || (defined __NES__)
	unsigned char sprShadow;
#endif
	int iVel, iVelMax, iAng1, iAng2, iCos, iSin, iTmp, ticks;
	unsigned char iCtrl, iRotMax, iJoy, collisions; 
	unsigned char res, lastKey, iJmp, iDir, iSpr, i, j;
	unsigned int lapTime;
	char chatting = 0;
	
	// Flush key entries
	while (kbhit()) { cgetc(); }

    // Reset game
	GameReset();
	
	// Notify server that game is ready?
	if (gameMode == MODE_ONLINE) { 
		res = ClientReady(); 
        if (res == ERR_TIMEOUT) { 
			PrintTimedOut();
			return 0; 
		} else if (res == EVENT_MAP) { 
			return 1;
		}
    }		

	// Initialize Clocks
	gameFrame = 0;
	gameClock = clock();
#if defined __APPLE2__
	clk += 1;
#endif				
	
    // Main loop of Application
	while (1) {
		// Get ticks and update clock
		ticks = (int)(clock()-gameClock);
		gameClock = clock();
		#if defined __LYNX__
			UpdateDisplay(); // Refresh Lynx screen
		#endif
	
		// Update cars
		for (i=0; i<MAX_PLAYERS; ++i) {
			// Player available?
			if (!PlayerAvailable(i)) { continue; }
		#if defined __APPLE2__	// Regulate clock approximately...
			if (!(gameFrame%3)) { clk += 2; } else { clk += 1; }
		#endif
			// Get player parameters
			iCtrl = controlIndex[i];
			iCar = &cars[i];
			iX = iCar->x2;
			iY = iCar->y2;
			iAng1 = iCar->ang1;
			iAng2 = iCar->ang2;
			iVel = iCar->vel;
			iJmp = 0;	
			
			// Get customized physics parameters
			if (iCtrl > 3) {
				iVelMax = velMax[3];
				iRotMax = rotMax[3];
			} else {
				j = iCtrl-1;
				iVelMax = velMax[j];
				iRotMax = rotMax[j];
			}				
			
			// Gently LERP network players
			if (iCtrl == NET_CONTROL) {
				if (abs(iCar->x1)+abs(iCar->y1) > LERP_THRESHOLD) {
					iX += iCar->x1; iCar->x1 = 0;
					iY += iCar->y1; iCar->y1 = 0;
				} else {
					if      (iCar->x1 > 0) { if (iCar->x1 >= ticks) { iX += ticks; iCar->x1 -= ticks; } else { iX++; iCar->x1--; } }
					else if (iCar->x1 < 0) { if (iCar->x1 <= ticks) { iX -= ticks; iCar->x1 += ticks; } else { iX--; iCar->x1++; } }
					if      (iCar->y1 > 0) { if (iCar->y1 >= ticks) { iY += ticks; iCar->y1 -= ticks; } else { iY++; iCar->y1--; } }
					else if (iCar->y1 < 0) { if (iCar->y1 <= ticks) { iY -= ticks; iCar->y1 += ticks; } else { iY--; iCar->y1++; } }
				}
			}

			// Fetch target waypoint
			GetWaypoint();

			// Decide the max velocity
			if (CheckRamps()) {
				// On ramp: check if jumping and increase max velocity
				if (iVel >= 450) { iCar->jmp = clock(); }
				iVelMax = VELRAMP;
			} else {
				// Jumping: maintain speed                        
				if ((clock()-iCar->jmp) < jmpTCK) {
					iVelMax = iVel;
					iJmp = 1;
				// Not jumping: check terrain
				} else {		  
					if (GetTerrain(iX, iY) != TERRAIN_ROAD)
						// Offroading: reduce speed
						iVelMax = VELMIN;                    
				}
			}             
			
			// Process Vehicle Control (when not jumping)
			if (!iJmp) {
				// Process Human Players
				iTmp = MIN(45, rotRate*ticks);
				if (iCtrl > 3) {
					// State provided by network
					if (iCtrl == NET_CONTROL) {
						iJoy = iCar->joy;
				#if defined __APPLE2__
					// Process analog signal from paddles
					} else {
						iJoy = 255-JOY_BTN1*GetButton(iCtrl-4);
						res = GetPaddle(iCtrl-4);
						if (res > 159) { 
							iAng2 -= ((res-127)/33u)*ticks;
							iJoy -= JOY_RIGHT;
						} else if (res < 93) {
							iAng2 += ((127-res)/33u)*ticks;
							iJoy -= JOY_LEFT;
						}
					}
				#else
					// Process digital signal from joysticks
					} else {
					#if defined(__LYNX__) || defined(__NES__)						
					  if (chatting)
						iJoy = 255;
					  else
					#endif  
						iJoy = GetJoy(iCtrl-4);
					}
					
					// Process joystick input
					if (!(iJoy & JOY_LEFT))  iAng2 += iTmp; 
					if (!(iJoy & JOY_RIGHT)) iAng2 -= iTmp;
				#endif						
					if (!(iJoy & JOY_BTN1)) { 
						iVel += accRate*ticks;
					} else {
						iVel -= decRate*ticks;					
						if (iVel < 0) { iVel = 0; }
					}
				// Process AI Players (only when racing)
				} else if (gameStep > STEP_WARMUP) {
					// Compute velocity
					iVel += accRate*ticks;

					// Check navigation? (not every frames)
					if (gameFrame % MAX_PLAYERS == i)
						iCar->ang3 = GetWaypointAngle(i);
					
					// Lerp to navigation target
					iAng2 = LerpAngle(iAng2, iCar->ang3, 3*iTmp);
				}
			}
		#if (defined __APPLE2__) || (defined __ORIC__)		// Simplified physics on slower systems...
			// Constrain angle range
			if (iAng2 > 360) { iAng2 -= 360; } else if (iAng2 < 0) { iAng2 += 360; }
			
			// Constrain velocity
			if (iCtrl > 3 && iAng1 != iAng2) {
				if (iVel > (iVelMax-45)) { iVel = (iVelMax-45); }								
			} else {
				if (iVel > iVelMax) { iVel = iVelMax; }								
			}
			iAng1 = iAng2;
			
			// Round sprite angle to nearest 22.5* sector
			iSpr = (iAng2+12)/23u;
			if (iSpr>15) { iSpr=0; }			
			iDir = iSpr;
		#else							
			// Lerp trajectory angle to create "drift effect"
			iAng1 = LerpAngle(iAng1, iAng2, iRotMax*ticks);	
			
			// Constrain velocity (and slow down when drifting)
			if (iCtrl > 3) {	
				if (iVel > (iVelMax - deltaAngle)) { iVel = (iVelMax - deltaAngle); }
			} else {
				if (iVel > iVelMax) { iVel = iVelMax; }
			}
		
			// Constrain angle range
			if (iAng1 > 360) { iAng1 -= 360; } else if (iAng1 < 0) { iAng1 += 360; }
			if (iAng2 > 360) { iAng2 -= 360; } else if (iAng2 < 0) { iAng2 += 360; }
			
			// Round trajectory angle to nearest 22.5* sector				
			iDir = (iAng1+12)/23u;
			if (iDir>15) { iDir=0; }				
			
			// Round sprite angle to nearest 22.5* sector
			iSpr = (iAng2+12)/23u;
			if (iSpr>15) { iSpr=0; }							
		#endif
			
			// Compute direction vector
			iCos = cos[iDir];
			iSin = sin[iDir];
		
			// Compute next position
		#if defined __ORIC__
			iTmp = ticks * iVel / 16u;
			iX += iCar->impx + ( iTmp * iCos ) / TCK_PER_SEC;
			iY += iCar->impy - ( iTmp * iSin ) / TCK_PER_SEC;
		#else				
			iTmp = ticks * iVel / 4u;
			iX += iCar->impx + ( iTmp * iCos ) / tck4;
			iY += iCar->impy - ( iTmp * iSin ) / tck4;
		#endif                
			// Reset impulse
			iCar->impx = 0;
			iCar->impy = 0;
			
			// Check map boundaries				
			if (iX < XMIN) { iX = XMIN; } else if (iX > XMAX) { iX = XMAX; }
			if (iY < YMIN) { iY = YMIN; } else if (iY > YMAX) { iY = YMAX; }
					
			// Compute sprite location
		#if defined __APPLE2__
			iSpr += i*16;
			spriteX = (iX*7)/128u;
			spriteY = (iY*3)/25u;
		#elif defined __ATARI__
			spriteX = iX/16u + 45; 
			spriteY = iY/8u;
		#elif defined __ORIC__
			spriteX = iX/32u;	
			spriteY = iY/8u;							
		#elif defined __CBM__					
			spriteX = iX/8u; 
			spriteY = iY/8u;
		#elif defined __LYNX__
			spriteX = iX/16u; 
			spriteY = iY/16u;	
		#elif defined __NES__
			spriteX =  iX/10u; 
			spriteY = (iY*3)/25u+24;
		#endif				
		
			// Check if we hit a wall...
			if (GetTerrain(iX, iY) == TERRAIN_WALL) {
				// Return to previous position
				if (iVel > VELMIN) { iVel = VELMIN; }
				if (iCtrl > 3) {
					if (iJmp) { iJmp = 0; }
					iX = iCar->x2;
					iY = iCar->y2;
					BumpSFX();
				}
			}

			// Display sprite
			if (iJmp) {
			#if (defined __ATARI__) || (defined __C64__) || (defined __LYNX__) || (defined __NES__)
				sprShadow |= (1<<i);
				SetSprite(SPR2_SLOT+i, 17);	// Display shadow Sprite
				EnableSprite(SPR2_SLOT+i);
				spriteY -= 2;		// Offset player sprite vertically
			} else {
				if (sprShadow & (1<<i)) { 
					DisableSprite(SPR2_SLOT+i); 
					sprShadow &= ~(1<<i);
				}
			# else
				spriteY -= 3;		// Offset player sprite vertically					
			#endif
			}
			
			// Display main sprite
		#if defined MULTICOLOR
			SetMultiColorSprite(2*i, iSpr);	// Car body and tires
		#else  
			SetSprite(i, iSpr);		
		#endif			
		
			// Check collisions
		#if defined MULTICOLOR
			collisions = COLLISIONS(2*i);
		#else  			
			collisions = COLLISIONS(i);
		#endif			
			if (collisions) {
				for (j=0; j<MAX_PLAYERS; j++) {
					if (i != j) {
					#if defined MULTICOLOR
						if (COLLIDING(collisions,2*j)) {
					#else  			
						if (COLLIDING(collisions,j)) {
					#endif			
							// Check neither are flying
							jCar = &cars[j];
							if (iJmp || (clock()-jCar->jmp) < jmpTCK) { continue; }
							// Apply impulse to other car, and reduce own velocity
							if ( (iCos*(jCar->x2 - iX) - iSin*(jCar->y2 - iY)) > 0) {
								if (iVel > VELMIN) { iVel = VELMIN; }
								jCar->impx = iCos/2;
								jCar->impy = -iSin/2;		
								BumpSFX();
							} 
						}
					}
				}
			}		
			
			// Update sound
		#if defined(__LYNX__) || defined(__CBM__)
			if (iJmp)	
				JumpSFX(i);
			else
			if (iVel < VELDRIFT || deltaAngle < 25)
		#endif
				EngineSFX(i, iVel);
		#if defined(__LYNX__) || defined(__CBM__)
			else
				ScreechSFX(i);					
		#endif
			
			// Update car position
			iCar->x2 = iX;
			iCar->y2 = iY;
			iCar->ang1 = iAng1;
			iCar->ang2 = iAng2;
			iCar->vel = iVel;
			iCar->joy = iJoy;

			// Check navigation
			if (iCtrl != NET_CONTROL && gameStep > STEP_WARMUP && (gameFrame&1) == (i&1)) {
				// Check current cylinder
				if (CheckWaypoint()) {
					iCar->way++;
					if ( iCar->way/2u == numWays) { iCar->way = 0; }
					if ( iCar->way == 1) { 
						// Increment laps
						iCar->lap += 1;
						
						// Local: Process lap
						if (gameMode == MODE_LOCAL && iCar->lap > 0) {							
							// Play lap sound and update UI
							BleepSFX(128); 
							PrintLap(i);
							
							// Compute lap time
							lapTime = gameClock - lapClock[i];
							lapClock[i] = gameClock;
							if (lapTime < lapBest[i])
								lapBest[i] = lapTime;

							// Check for winner
							if (iCar->lap == lapGoal) {
								PrintScores();
								return 1; 
							}	
						}
					}
				}
				// Update old car position
				iCar->x1 = iCar->x2;
				iCar->y1 = iCar->y2;						
			}			
		}

		// Process sound effects
		UpdateSFX();
		
		// Check Keyboard Press
	#if defined(__LYNX__) || defined(__NES__)
		if (GetJoy(0) & JOY_BTN2)
			LockBackButton = 0;
		if (kbhit() || KeyboardOverlayHit() || (!(GetJoy(0) & JOY_BTN2) && !LockBackButton)) {
			if (chatting) {
				lastKey = GetKeyboardOverlay();
			} else if (!(GetJoy(0) & JOY_BTN2)) {
				lastKey = KB_CHAT;
			} else {
				lastKey = cgetc();
			}
			
			// Special functions (flip screen, pause game)
			if (!chatting) {
				switch (lastKey) {
				case KB_PAUSE: 
					for (j=0; j<MAX_PLAYERS; ++j) { DisableSprite(j); }
					BackupRestorePauseBg(0);			// Backup background GFX
					gamePaused = 1; PauseMusic(1); 		// Disable music/sound
					lastKey = MenuPause(); 				// Enter pause menu loop
					while (kbhit()) cgetc();			// Flush keyboard
					gamePaused = 0; PauseMusic(0);		// Resume music/sound
					BackupRestorePauseBg(1); 			// Restore background GFX
					for (j=0; j<MAX_PLAYERS; ++j) { 
						if (PlayerAvailable(j)) {
							lapClock[j] += (clock()-gameClock);  // Adjust lap clocks
							EnableSprite(j); 
						}
					}
					gameClock = clock();
					break;
			#if defined(__LYNX__)		
				case KB_MUSIC:
					NextMusic(1);
					break;
				case KB_FLIP:
					SuzyFlip();
					break;
			#endif
				}
			}
	#else
		if (kbhit()) {
			lastKey = cgetc();
	#endif
			// Process Chat?
			if (chatting) {
			#ifndef __NES__	
				inkColor = inkColors[clIndex];
				txtX = 0; txtY = CHAT_ROW;
				if (InputStr(19, chatBuffer, 19, lastKey)) {
					// Return was pressed
					if (chatBuffer[0] != 0) { ClientEvent(EVENT_CHAT); }
					chatting = 0;
					BackupRestoreChatRow(1);
				#if defined __LYNX__
					HideKeyboardOverlay();
					LockBackButton = 1;
				#endif						
				}
			#endif
			} else {
				// Start Race
				if (lastKey == KB_START) {
					if (gameMode == MODE_ONLINE) {
						ClientEvent(EVENT_RACE);
					} else {
						gameStep = STEP_RACE;
						GameRace();
					}	
				}
				// Next Map
				if (lastKey == KB_NEXT) {
					if (gameMode == MODE_ONLINE) {
						ClientEvent(EVENT_MAP);
					} else {
						if (gameStep > STEP_WARMUP) { PrintScores(); } 
						return 1; 
					}
				}					
				// Enable chat
				if (gameMode == MODE_ONLINE && lastKey == KB_CHAT) {
				#ifndef __NES__	
				  #if defined __LYNX__
					BackupRestoreChatRow(0);
					ShowKeyboardOverlay();
				  #endif
					chatting = 1;
					chatBuffer[0] = 0;
					txtX = 0; txtY = CHAT_ROW;
					InputStr(19, chatBuffer, 19, 0);
				#endif
				}
				// Quit game
				if (lastKey == KB_QUIT) {
					if (gameMode == MODE_ONLINE) { 
						ClientLeave(); 
						ServerDisconnect();
					}
					return 0; 
				}
			#if defined __ATARIXL__
				if (lastKey == KB_G) bmpToggle ^= 2; // Toggle Graphic Mode
			#elif defined __ORIC__				
				if (lastKey == KB_V) volumeReduced = 1-volumeReduced; // Toggle Graphic Mode
			#endif
			}
		}
		
		// Update Network
		if (gameMode == MODE_ONLINE) {
		#if defined __LYNX__
			if (pauseEvt) {
				j = pauseEvt;
				pauseEvt = 0;
			} else {
				j = NetworkUpdate();
			}
		#else
			j = NetworkUpdate();
		#endif
			if (j == EVENT_RACE) {
				// Start Race
				if (chatting) {
					chatting = 0;
					BackupRestoreChatRow(1);
				#if defined __LYNX__
					HideKeyboardOverlay();						
				#endif						
				}
				gameStep = svStep;
				res = GameRace();
				if (res != 1) { 
					if (res == ERR_TIMEOUT) { return 0; } else { return 1; }
				}
			} else if (j == EVENT_MAP) {
				// Go to next Map
				if (gameStep > STEP_WARMUP) { PrintScores(); }
				gameMap = svMap;
				gameStep = svStep;
				return 1;
			} else if (j == ERR_TIMEOUT) {
				// Time-out error
				PrintTimedOut();
				return 0;
			}
		}
		
		// Draw FPS performance?
	#ifdef DEBUG_FPS
		if (gameFrame%60 == 59) { DrawFPS(gameFrame); BleepSFX(64); }
	#endif
		gameFrame++;
	}
}
