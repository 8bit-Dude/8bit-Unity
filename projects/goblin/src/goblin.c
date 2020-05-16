
#include "scene.h"

void RunScene(void)
{
	unsigned char joy, test;
	unsigned char sceneSearch, sceneIndex = 255, sceneInteract = 255;
	unsigned char mouseL = 0, mouseMotion = 0, mouseAction = 0;
	unsigned int mouseX = 160, mouseY = 100;
	unsigned int goalX = 180, goalY = 130;	
	unsigned char unit1Frame = frameWaitLeft, waitFrame = frameWaitLeft;
	unsigned int unit1X = 180, unit1Y = 130;	
	signed int interX, interY, deltaX, deltaY;
	clock_t gameClock = clock();
	
	while (1) {
	#if defined __APPLE2__
		tick();	// Simulate clock on Apple 2
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
		// Move cursor
		mouseMotion = 0;
		joy = GetJoy(0);
		if (joy != 255) { mouseMotion = 1; }
		if (!(joy & JOY_UP))    { mouseY -= 2; if (mouseY > 200) mouseY = 0; }
		if (!(joy & JOY_DOWN))  { mouseY += 2; if (mouseY > 200) mouseY = 200; }
		if (!(joy & JOY_LEFT))  { mouseX -= 2; if (mouseX > 320) mouseX = 0; }
		if (!(joy & JOY_RIGHT)) { mouseX += 2; if (mouseX > 320) mouseX = 320; }
		if (!(joy & JOY_BTN1))  { mouseL = 1; } else { mouseL = 0; mouseAction = 0; }
		
		// Search scene
		if (mouseMotion) {
			sceneSearch = SearchScene(mouseX, mouseY);			
			if (sceneSearch != sceneIndex) {
				sceneIndex = sceneSearch;
				if (sceneIndex == 255) {
					DrawPanel(0, CHR_ROWS-2, CHR_COLS-8, CHR_ROWS-1);
				} else {
					PrintStr(0, CHR_ROWS-2, interacts[sceneIndex].label);
				}
			}
		}
		
		// Process commands
		if (mouseL && !mouseAction) {
	 		// Record action
			mouseAction = 1;
			BumpSFX();
			
	 		// Update mouse cursor
			LocateSprite(mouseX+4, mouseY+4);
			SetSprite(0, mouseL);
		#if defined __LYNX__
			UpdateDisplay(); // Refresh Lynx screen
		#endif
		
			// Compute goal coordinates
			test = IntersectSegmentPolygon(unit1X, unit1Y, mouseX, mouseY, MAX_POLYGON, polygonX, polygonY, &interX, &interY);
			if (test && (unit1X != interX || unit1Y != interY)) { 	// Check that we are not stuck on a polygon segment
				goalX = interX;
				goalY = interY;
			} else {
				// Move directly to mouse cursor (if in allowed area, and not crossing other parts of polygon)
				if (test < 2 && PointInsidePolygon(mouseX, mouseY, MAX_POLYGON, polygonX, polygonY)) {
					goalX = mouseX;
					goalY = mouseY;
				}
			}
			
			// Save index, for interaction when we reach position
			sceneInteract = sceneIndex;
		}
				
		// Process unit motion
		if (clock()>gameClock+5) {
			gameClock = clock();
			deltaX = goalX - unit1X;
			deltaY = goalY - unit1Y;
			if (deltaX || deltaY) {
				// Move in steps of 3 max.
				unit1X += SIGN(deltaX) * MIN(ABS(deltaX),3); 
				unit1Y += SIGN(deltaY) * MIN(ABS(deltaY),3);
				if (unit1X > 320) unit1X = 0;
				if (unit1Y > 200) unit1Y = 0;
				
				// Update frame number
				if (deltaX > 0) {
					if (unit1Frame < frameWalkRightBeg) unit1Frame = frameWalkRightBeg;
					unit1Frame += 1; if (unit1Frame > frameWalkRightEnd) unit1Frame = frameWalkRightBeg;
					waitFrame = frameWaitRight;
				} else {
					if (unit1Frame > frameWalkLeftEnd) { unit1Frame = frameWalkLeftBeg; }
					unit1Frame += 1; if (unit1Frame > frameWalkLeftEnd) unit1Frame = frameWalkLeftBeg;
					waitFrame = frameWaitLeft;
				}
			} else {
				// Process trigger (if any) and set wait frame
				if (sceneInteract != 255) {
					ProcessInteract(sceneInteract);
					sceneInteract = 255;
				}
				unit1Frame = waitFrame;
			}
		}
	
		// Set sprites
		LocateSprite(mouseX+4, mouseY+4);
		SetSprite(0, mouseL);
		LocateSprite(unit1X, unit1Y-10);
	#if (defined __ATARI__) || (defined __CBM__)
		SetSprite(1, unit1Frame);		// Unit color #1
		SetSprite(2, unit1Frame+14);	// Unit color #2
		SetSprite(3, unit1Frame+28);	// Unit color #3
		SetSprite(4, unit1Frame+42);	// Unit color #4
	#else
		SetSprite(1, unit1Frame);
	#endif
	}	
}

int main(void) 
{
	// Set text mode colors
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	
	// Initialize sfx, bitmap, sprites
	InitSFX();
	InitBitmap();
	InitSprites(spriteFrames, spriteCols, spriteRows, spriteColors);

	// Clear and show bitmap
	LoadBitmap("scene1.map");
	EnterBitmapMode();
	
	// Enable sprites
	EnableSprite(0);  // Mouse cursor
	EnableSprite(1);  // Unit #1
#if defined __APPLE2__
	CropSprite(0,12); // Mouse cursor only occupies half frame
#elif defined __ATARI__
	EnableSprite(2);  // Unit #1 (extra color)
	EnableSprite(3);  // Unit #1 (extra color)
	EnableSprite(4);  // Unit #1 (extra color)
#elif defined __ORIC__
	CropSprite(0,8);  // Mouse cursor only occupies half frame
#elif defined __CBM__ 
	EnableSprite(2);  // Unit #1 (extra color)	
	EnableSprite(3);  // Unit #1 (extra color)
	EnableSprite(4);  // Unit #1 (extra color)
	DoubleHeightSprite(1, 1);
	DoubleHeightSprite(2, 1);
	DoubleHeightSprite(3, 1);
	DoubleHeightSprite(4, 1);
#endif	

	// Load and play music
#ifndef __APPLE2__
	LoadMusic("music.dat", MUSICRAM);
	PlayMusic(MUSICRAM);
#endif
	
	// Init and Run Scene
	InitScene();
	PrintInventory();
	RunScene();
	
	// Black-out screen and clear key
	DisableSprite(-1);	// "-1" disables all sprites
	ExitBitmapMode();
	StopSFX();
	
    // Done
    return EXIT_SUCCESS;	
}
