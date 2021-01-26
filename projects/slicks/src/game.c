
#include "definitions.h"

#if defined __APPLE2__
 #if defined __DHR__
  #define RACE_ROAD LGREY
  #define RACE_MARK YELLOW
  #define RACE_WALL PURPLE
 #else
  #define RACE_ROAD BLACK
  #define RACE_MARK WHITE
  #define RACE_WALL PURPLE
 #endif	 
#elif defined __ATARI__
  #define RACE_ROAD BLACK
  #define RACE_MARK GREY
  #define RACE_WALL BROWN
  #define SPR2_SLOT 7
#elif defined __ORIC__
  #define RACE_ROAD BLACK
  #define RACE_MARK LGREEN
  #define RACE_WALL PURPLE
  #define SPR2_SLOT 4
#elif defined __CBM__
  #define RACE_ROAD LGREY
  #define RACE_MARK YELLOW
  #define RACE_WALL PURPLE
  #define SPR2_SLOT 4
#elif defined __LYNX__
  #define RACE_ROAD GREY
  #define RACE_MARK YELLOW
  #define RACE_WALL PURPLE
  #define SPR2_SLOT 4
  void BackupPauseBg(void);
  void RestorePauseBg(void);
  unsigned char MenuPause(void);
  extern unsigned char pauseEvt;
  extern unsigned char gamePaused;
#endif

// See slicks.c
extern unsigned char inkColors[];

// See interface.c
extern unsigned char controlIndex[MAX_PLAYERS];
extern unsigned char controlBackup[MAX_PLAYERS];
extern unsigned char lapNumber[LEN_LAPS];
extern const char *mapList[LEN_MAPS];
#if defined(__CBM__) || defined(__LYNX__)
  extern unsigned char paperBuffer;
#endif
#if defined __LYNX__
  void NextMusic(unsigned char blank);
#endif

// See navigation.c
extern Vehicle cars[MAX_PLAYERS];
extern unsigned char numWays;

// See network.c
extern unsigned char clIndex;
extern unsigned char svUsers[MAX_PLAYERS][5];
extern unsigned char svMap, svStep; 
extern char chatBuffer[20];

// See Unity/Lynx/Suzy.s
void __fastcall__ SuzyFlip(void);

// Map boundaries
int xMin = 3*8;
int yMin = 11*8;
int xMax = 317*8;
int yMax = 189*8;

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
  #define LIGHT_X  118
  #define LIGHT_Y   24
  #define LIGHT_SP  36
  char rotRate = 2;
#else
  #define LIGHT_X  132
  #define LIGHT_Y   24
  #define LIGHT_SP  24
  char rotRate = 3;
#endif
const char rotMax[4] = { 4, 5, 6, 2 };
const int velMin = 200;
const int velMax[4] = { 450, 510, 570, 600 };
const int velDrift = 450;
const int velRamp = 800;

// Fast tables for cos/sin
const int cos[16] = {16,14,11,6,0,-6,-11,-14,-16,-14,-11,-6,0,6,11,14};
const int sin[16] = {0,6,11,14,16,14,11,6,0,-6,-11,-14,-16,-14,-11,-6};

// Clock management
clock_t gameClock;
unsigned long gameFrame;

unsigned char PlayerAvailable(unsigned char i)
{
	// CPU players not available during warmup phase
	if (controlIndex[i]) {
		if ((gameStep == STEP_WARMUP) && (controlIndex[i] < 4))
			return 0;
		else
			return 1;
	}
	return 0;
}

// Reset Game
void GameReset()
{
    unsigned char i,f;
    
    // Reset Game State
	ResetLineUp();
	
    // Reset Players
    for (i=0; i<MAX_PLAYERS; ++i) {        
        // Reset SFX
        EngineSFX(i, 0);
	
        // Player available?
		if (!PlayerAvailable(i)) { continue; }

		// Reset laps and sprite
		PrintStr((i+2)*8u-3, CHR_ROWS-1, "  ");
		f = ((cars[i].ang1+12)%(360))/23u;
	#if defined __APPLE2__
		f += i*16u;
		spriteX = (cars[i].x2*7u)/128u;
		spriteY = (cars[i].y2*3u)/25u;
	#elif defined __ATARI__
		spriteX = cars[i].x2/16u + 45; 
		spriteY = cars[i].y2/8u + 32;		
	#elif defined __ORIC__
		spriteX = cars[i].x2/32u;	
		spriteY = cars[i].y2/8u;			
	#elif defined __CBM__
		spriteX = cars[i].x2/8u; 
		spriteY = cars[i].y2/8u;
	#elif defined __LYNX__
		spriteX = cars[i].x2/16u; 
		spriteY = cars[i].y2/16u;
	#endif
		SetSprite(i, f);
		EnableSprite(i);
    }
    
	// Display warmup message
	inkColor = WHITE; 
#if defined(__CBM__) || defined(__LYNX__)
	paperColor = paperBuffer;
#else
	paperColor = BLACK; 
#endif
	if (gameStep == STEP_WARMUP) {
        if (gameMode == MODE_ONLINE) {
		#if defined __CBM__
            PrintBuffer("  WARMUP: F1-RACE, F3-NEXT MAP, C-CHAT  ");   
		#elif defined __LYNX__
            PrintBuffer("   WARMUP: OP1-RACE, PAU-MENU, B-CHAT   ");   
		#else
            PrintBuffer("   WARMUP: 1-RACE, 2-NEXT MAP, C-CHAT   ");   
		#endif
        } else {
		#if defined __CBM__
            PrintBuffer("  WARMUP: F1-RACE, F3-NEXT MAP, Q-Quit  ");   
		#elif defined __LYNX__
            PrintBuffer("  WARMUP: OP1-RACE, PAU-MENU, RS-Quit   ");   
		#else
            PrintBuffer("   WARMUP: 1-RACE, 2-NEXT MAP, Q-Quit   ");   
		#endif
        }
	} else {
		PrintRace();
	}	
	paperColor = BLACK;
}

// Initialize Game
void GameInit(const char* map)
{
	char i, slot, len, buffer[12];
	
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
	memcpy(&buffer[len], ".img", 4);
	buffer[len+4] = 0;
	
	// Load Bitmap and backup chat row
	LoadBitmap(&buffer[0]);
	BackupChatRow();

	// Get paper color of top buffer
	pixelX = 0; pixelY = 0;
#if defined(__CBM__) || defined(__LYNX__)
	paperBuffer = GetPixel();	
#endif		
	// Print map name in lower left corner
	inkColor = WHITE; paperColor = BLACK;
	PrintStr(0, CHR_ROWS-1, mapList[gameMap]);

	// Load Navigation
	memcpy(&buffer[len], ".nav", 4);
	LoadNavigation(&buffer[0]);
	
    // Some extra logics depending on the game mode
	if (gameMode == MODE_LOCAL) {
        // Setup players
        for (i=0; i<MAX_PLAYERS; ++i) {
            // Print player numbers in score board
            if (controlIndex[i]) {
				slot = (i+1)*8u;
                inkColor = inkColors[i]; 
			#if defined __ORIC__
				SetAttributes(slot-1, CHR_ROWS-1, inkColor);
			#endif					
				PrintChr(slot+1, CHR_ROWS-1, &charDigit[(i+1)*3]);
                PrintChr(slot, CHR_ROWS-1, &charLetter[15*3]);	// 'P'
            }
        }
        // Initiate laps
		lapGoal = lapNumber[lapIndex];
	} else {
        // Disable all players and reset user names...
        for (i=0; i<MAX_PLAYERS; ++i) { 
			svUsers[i][0] = 0;
            controlIndex[i] = 0;
        }
		// Assign 1st Controller to Client
		controlIndex[clIndex] = controlBackup[0];
    }
#if defined __LYNX__
	// Overlay for in-game chat
	SetKeyboardOverlay(13,13);
#endif	
}

// Game step
unsigned char GameRace()
{
    // Reset Game
	unsigned char i,res;
    GameReset();

    // Show light sprites
#ifndef __APPLE2__
    for (i=SPR2_SLOT; i<SPR2_SLOT+3; ++i) {
		LocateSprite(LIGHT_X+(i-SPR2_SLOT)*LIGHT_SP, LIGHT_Y);
	#if defined __ATARI__ 
		RecolorSprite(i, 0, 0x08);
		SetSprite(i, 16);
	#elif defined __ORIC__ 
		RecolorSprite(i, 0, SPR_AIC);
		SetSprite(i, 16);
	#elif defined __CBM__
		RecolorSprite(i, 0, LGREY);
		SetSprite(i, 16);
	#elif defined __LYNX__
		RecolorSprite(i, 7, 0xef);
		SetSprite(i, 16);
		UpdateDisplay();
	#endif		
        EnableSprite(i);
    }
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
#if defined __ORIC__ 
	LocateSprite(LIGHT_X, 24);	
	RecolorSprite(4, 0, SPR_RED);
	SetSprite(4, 16);
#elif defined __ATARI__  
	RecolorSprite(7, 0, 0x22);  
#elif defined __CBM__  
	RecolorSprite(4, 0, RED);  
#elif defined __LYNX__
	RecolorSprite(4, 7, 0xbf);
	UpdateDisplay();
#endif	
	BleepSFX(64); 
#ifndef __APPLE2__
    sleep(1);
#else
	wait(30);
#endif	

    // Orange light	
#if defined __ORIC__ 
	LocateSprite(LIGHT_X+LIGHT_SP, 24);	
	RecolorSprite(5, 0, SPR_RED);
	SetSprite(5, 16);	
#elif defined __ATARI__  
	RecolorSprite(8, 0, 0x1a);  
#elif defined __CBM__  
	RecolorSprite(5, 0, ORANGE);  
#elif defined __LYNX__
	RecolorSprite(5, 7, 0x8f);	
	UpdateDisplay();
#endif	
	BleepSFX(64); 
#ifndef __APPLE2__
    sleep(1);
#else
	wait(30);
#endif	

    // Green light
#if defined __ORIC__ 
	LocateSprite(LIGHT_X+2*LIGHT_SP, 24);	
	RecolorSprite(6, 0, SPR_GREEN);
	SetSprite(6, 16);	
#elif defined __ATARI__  
	RecolorSprite(9, 0, 0xc4);  
#elif defined __CBM__  
	RecolorSprite(6, 0, GREEN);  
#elif defined __LYNX__
	RecolorSprite(6, 7, 0x4f);	
	UpdateDisplay();
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
#ifndef __APPLE2__
    for (i=SPR2_SLOT; i<SPR2_SLOT+3; ++i) {
	#if defined __ATARI__  
		RecolorSprite(i, 0, 0x22);  
	#elif defined __CBM__  
		RecolorSprite(i, 0, BLACK);  
	#elif defined __LYNX__
		RecolorSprite(i, 7, 0xef);
		UpdateDisplay();
	#endif			
		DisableSprite(i);
	}
#endif	

	// Reset game clock
	gameClock = clock();
#if defined __APPLE2__
	clk += 1;
#endif	
	return 1;
}

// Angle LERPing
int deltaAngle;
int LerpAngle(int iAng1, int iAng2, int dAng)
{
	// LERP trajectory
	int iSign = 1;
	deltaAngle = iAng2 - iAng1;
	if (deltaAngle < 0) {
		iSign = -1;
		deltaAngle = -deltaAngle;
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

// Game loop (for 1 round)
char GameLoop()
{		
	// Game Management
	Vehicle *car;
#if (defined __C64__) || (defined __LYNX__)
	unsigned char sprShadow;
#endif
	int iX, iY, iVel, iVelMax, iAng1, iAng2, iCos, iSin, iTmp, steps;
	unsigned char iCtrl, iRotMax, iJoy, iColor, collisions; 
	unsigned char res, lastKey, iJmp, iDir, iSpr, i, j;
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
		// Get steps and update clock
		steps = (int)(clock()-gameClock);
		gameClock = clock();
		#if defined __LYNX__
			UpdateDisplay(); // Refresh Lynx screen
		#endif
	
		// Update cars
		for (i=0; i<MAX_PLAYERS; ++i) {
			// Player available?
			if (!PlayerAvailable(i)) { continue; }
			iCtrl = controlIndex[i];
		#if defined __APPLE2__
			// Regulate clock approximately...
			if (gameMode == MODE_ONLINE) {
			  #if define __DHR_	
				if (gameFrame%3) { clk += 1; } else { clk += 2; }
			  #else
				if (gameFrame%4) { clk += 1; } else { clk += 2; }
			  #endif
			} else {
			  #if define __DHR_	
				if (gameFrame%4) { clk += 2; } else { clk += 1; }
			  #else
				if (gameFrame%3) { clk += 2; } else { clk += 1; }
			  #endif
			}				
		#endif
			// Get player parameters
			car = &cars[i];
			iX = car->x2;
			iY = car->y2;
			iAng1 = car->ang1;
			iAng2 = car->ang2;
			iVel = car->vel;
			iJmp = 0;	
			
			// Get customized physics parameters
			if (iCtrl > 3) {
				iVelMax = velMax[3];
				iRotMax = rotMax[3];
			} else {
				iVelMax = velMax[iCtrl-1];
				iRotMax = rotMax[iCtrl-1];
			}				
			
			// Gently LERP network players
			if (iCtrl == NET_CONTROL) {
				if (abs(car->x1)+abs(car->y1) > LERP_THRESHOLD) {
					iX += car->x1; car->x1 = 0;
					iY += car->y1; car->y1 = 0;
				} else {
					if      (car->x1 > 0) { if (car->x1 >= steps) { iX += steps; car->x1 -= steps; } else { iX++; car->x1--; } }
					else if (car->x1 < 0) { if (car->x1 <= steps) { iX -= steps; car->x1 += steps; } else { iX--; car->x1++; } }
					if      (car->y1 > 0) { if (car->y1 >= steps) { iY += steps; car->y1 -= steps; } else { iY++; car->y1--; } }
					else if (car->y1 < 0) { if (car->y1 <= steps) { iY -= steps; car->y1 += steps; } else { iY--; car->y1++; } }
				}
			}

			// Get background color
			LocatePixel(iX/8u, iY/8u);
			iColor = GetPixel();

			// Decide the max velocity
			if (CheckRamps(car)) {
				// On ramp: check if jumping and increase max velocity
				if (iVel >= 450) { car->jmp = clock(); }
				iVelMax = velRamp;
			} else {
				// Jumping: maintain speed                        
				if ((clock()-car->jmp) < jmpTCK) {
					iVelMax = iVel;
					iJmp = 1;
				} else {
					// Not on track: reduce speed
					if (iColor != RACE_ROAD && iColor != RACE_MARK) {
						iVelMax = velMin;                    
					}
				}
			}             
			
			// Process Vehicle Control (when not jumping)
			if (!iJmp) {
				// Process Human Players
				iTmp = MIN(45, rotRate*steps);
				if (iCtrl > 3) {
					// State provided by network
					if (iCtrl == NET_CONTROL) {
						iJoy = car->joy;
				#if defined __APPLE2__
					// Process analog signal from paddles
					} else {
						iJoy = 255-JOY_BTN1*GetButton(iCtrl-4);
						res = GetPaddle(iCtrl-4);
						if (res > 159) { 
							iAng2 -= ((res-127)/33u)*steps;
							iJoy -= JOY_RIGHT;
						} else if (res < 93) {
							iAng2 += ((127-res)/33u)*steps;
							iJoy -= JOY_LEFT;
						}
					}
				#else
					// Process digital signal from joysticks
					} else
						iJoy = GetJoy(iCtrl-4);

					// Process joystick input
					if (!(iJoy & JOY_LEFT))  iAng2 += iTmp; 
					if (!(iJoy & JOY_RIGHT)) iAng2 -= iTmp;
				#endif						
					if (!(iJoy & JOY_BTN1)) { 
						iVel += accRate*steps;
					} else {
						iVel -= decRate*steps;					
						if (iVel < 0) { iVel = 0; }
					}
				// Process AI Players (only when racing)
				} else if (gameStep > STEP_WARMUP) {
					// Compute velocity
					iVel += accRate*steps;

					// Check navigation? (not every frames)
					if (gameFrame % MAX_PLAYERS == i)					
						// Get angle to next waypoint
						car->ang3 = GetWaypointAngle(car);
					
					// Lerp to navigation target
					iAng2 = LerpAngle(iAng2, car->ang3, 2*iTmp);
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
			iAng1 = LerpAngle(iAng1, iAng2, iRotMax*steps);	
			
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
			iTmp = steps * iVel / 16u;
			iX += car->impx + ( iTmp * iCos ) / TCK_PER_SEC;
			iY += car->impy - ( iTmp * iSin ) / TCK_PER_SEC;
		#else				
			iTmp = steps * iVel / 4u;
			iX += car->impx + ( iTmp * iCos ) / tck4;
			iY += car->impy - ( iTmp * iSin ) / tck4;
		#endif                
			// Reset impulse
			car->impx = 0;
			car->impy = 0;
			
			// Check map boundaries				
			if (iX < xMin) { iX = xMin; } else if (iX > xMax) { iX = xMax; }
			if (iY < yMin) { iY = yMin; } else if (iY > yMax) { iY = yMax; }
					
			// Compute sprite location
		#if defined __APPLE2__
			iSpr += i*16;
			spriteX = (iX*7)/128u;
			spriteY = (iY*3)/25u;
		#elif defined __ATARI__
			spriteX = iX/16u + 45; 
			spriteY = iY/8u + 32;
		#elif defined __ORIC__
			spriteX = iX/32u;	
			spriteY = iY/8u;							
		#elif defined __CBM__					
			spriteX = iX/8u; 
			spriteY = iY/8u;
		#elif defined __LYNX__
			spriteX = iX/16u; 
			spriteY = iY/16u;				
		#endif				
		
			// Get again background color
			LocatePixel(iX/8u, iY/8u);
			iColor = GetPixel();
			if (iColor == RACE_WALL) {
				// Hit a wall: return to previous position
				if (iVel > velMin) { iVel = velMin; }
				if (iCtrl > 3) {
					iX = car->x2;
					iY = car->y2;
					BumpSFX();
				}
			}

			// Display sprite
		#if (defined __ATARI__)
			iTmp = 4+5*(i%2);
		#endif
			if (iJmp) {
			#if (defined __ATARI__)
				SetSprite(iTmp, 17);	// Display shadow Sprite
				EnableSprite(iTmp);
				spriteY -= 2;		    // Offset player sprite vertically
			} else {
				DisableSprite(iTmp); 
			#elif (defined __C64__) || (defined __LYNX__)
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
			SetSprite(i, iSpr);		// Display main sprite
		
			// Check collisions
			collisions = COLLISIONS(i);
			if (collisions) {
				for (j=0; j<MAX_PLAYERS; j++) {
					if (i != j) {
						if (COLLIDING(collisions,j)) {
							// Check neither are flying
							if (iJmp || (clock()-cars[j].jmp) < jmpTCK) { continue; }
							// Apply impulse to other car, and reduce own velocity
							if ( (iCos*(cars[j].x2 - iX) - iSin*(cars[j].y2 - iY)) > 0) {
								if (iVel > velMin) { iVel = velMin; }
								cars[j].impx = iCos/2;
								cars[j].impy = -iSin/2;		
								BumpSFX();
							} 
						}
					}
				}
			}
			
			// Update sound
		#if defined __LYNX__	
			if (iJmp) {		
				JumpSFX(i);
			} else
		#endif
			if (iVel < velDrift || deltaAngle < 25)
				EngineSFX(i, iVel);
		#if defined __LYNX__	
			else
				ScreechSFX(i);					
		#endif
			
			// Update car position
			car->x2 = iX;
			car->y2 = iY;
			car->ang1 = iAng1;
			car->ang2 = iAng2;
			car->vel = iVel;
			car->joy = iJoy;

			// Check navigation
			if (iCtrl != NET_CONTROL & gameStep > STEP_WARMUP & gameFrame%2 == i%2) {
				// Check current cylinder
				if (CheckWaypoint(car)) {
					car->way++;
					if ( car->way/2u == numWays) { car->way = 0; }
					if ( car->way == 1) { 
						// Increment laps
						car->lap += 1;
						
						// Local: Process lap
						if (gameMode == MODE_LOCAL) {
							// Play lap sound
							if (car->lap > 0) { BleepSFX(128); }

							// Update lap count
							PrintLap(i);
							
							// Check for winner
							if (car->lap == lapGoal) {
								PrintScores();
								return 1; 
							}								
						}
					}
				}
				// Update old car position
				car->x1 = car->x2;
				car->y1 = car->y2;						
			}			
		}				
		
		// Check Keyboard Press
	#if defined __LYNX__
		if (kbhit() || KeyboardOverlayHit() || !(GetJoy(0) & JOY_BTN2)) {
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
				case KB_FLIP:
					SuzyFlip();
					break;
				case KB_PAUSE: 
					for (j=0; j<MAX_PLAYERS; ++j) { DisableSprite(j); }
					gamePaused = 1; lynx_snd_pause(); BackupPauseBg();
					lastKey = MenuPause();
					gamePaused = 0; lynx_snd_continue(); RestorePauseBg(); 
					for (j=0; j<MAX_PLAYERS; ++j) { if (PlayerAvailable(j)) EnableSprite(j); }
					paperColor = BLACK;
					break;
				case KB_MUSIC:
					StopMusic();
					NextMusic(1);
					break;
				}
			}
	#else
		if (kbhit()) {
			lastKey = cgetc();
	#endif
			// Process Chat?
			if (chatting) {
				inkColor = inkColors[clIndex];
				if (InputStr(0, ROW_CHAT, 19, chatBuffer, 19, lastKey)) {
					// Return was pressed
					if (strlen(&chatBuffer[0]) > 0) { ClientEvent(EVENT_CHAT); }
					chatting = 0;
					RedrawChatRow();
				#if defined __LYNX__
					HideKeyboardOverlay();
				#endif						
				}
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
				if (gameMode == MODE_ONLINE & lastKey == KB_CHAT) {
				#if defined __LYNX__ 
					ShowKeyboardOverlay();
				#endif
					chatting = 1;
					chatBuffer[0] = 0;
					InputStr(0, ROW_CHAT, 19, chatBuffer, 19, 0);
				}
				// Quit game
				if (lastKey == KB_QUIT) {
					if (gameMode == MODE_ONLINE) { 
						ClientLeave(); 
						ServerDisconnect();
					}
					return 0; 
				}
			#ifdef  __ATARI__
				// Toggle RGB/BW
				if (lastKey == KB_G) frameBlending ^= 2;
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
					RedrawChatRow();
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
