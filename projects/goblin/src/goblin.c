
#include "scene.h"

void GameLoop(void)
{
	unsigned char joy, test;
	unsigned char sceneSearch, sceneIndex = 255, sceneInteract = 255, sceneItem = 255;
	unsigned char mouseL = 0, mouseMotion = 0, mouseAction = 0;
	unsigned int mouseX = 160, mouseY = 100;
	unsigned int clickX = 160, clickY = 100;
	unsigned int goalX = 180, goalY = 130;	
	unsigned char unitFrame = frameWaitLeft, waitFrame = frameWaitLeft;
	unsigned int unitX = 180, unitY = 130;	
	signed int interX, interY, deltaX, deltaY;
	clock_t gameClock = clock();
	
	while (1) {
	#if defined __APPLE2__
		clk += 1;  // Manually update clock on Apple 2
	#elif defined __LYNX__
		UpdateDisplay(); // Refresh Lynx screen
	#endif		
		// Move cursor
		mouseMotion = 0;
		joy = GetJoy(0);
		if (joy != 255) { mouseMotion = 1; }
		if (!(joy & JOY_UP))    { mouseY -= mouseStep; if (mouseY > 200) mouseY = 0; }
		if (!(joy & JOY_DOWN))  { mouseY += mouseStep; if (mouseY > 200) mouseY = 200; }
		if (!(joy & JOY_LEFT))  { mouseX -= mouseStep; if (mouseX > 320) mouseX = 0; }
		if (!(joy & JOY_RIGHT)) { mouseX += mouseStep; if (mouseX > 320) mouseX = 320; }
		if (!(joy & JOY_BTN1))  { mouseL = 1; } else { mouseL = 0; mouseAction = 0; }
		
		// Search scene
		if (mouseMotion) {
			sceneSearch = SearchScene(mouseX, mouseY);			
			if (sceneSearch != sceneIndex) {
				sceneIndex = sceneSearch;
				if (sceneIndex == 255) {
					PrintBlanks(0, CHR_ROWS-2, CHR_COLS-9, CHR_ROWS-2);
				} else {
					PrintInteract(sceneItem, interacts[sceneIndex].label);
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
		
			// Is mouse cursor in inventory area?
			if (mouseY > INVENTORY_Y) {
				sceneItem = SelectItem(mouseX, mouseY);
				if (sceneItem != 255) {
					PrintInteract(sceneItem, "\0");
				} else {
					PrintBlanks(0, CHR_ROWS-2, CHR_COLS-9, CHR_ROWS-2);
				}
			} else {
				// Get click coordinates
				if (sceneIndex == 255) {
					clickX = mouseX;
					clickY = mouseY;
				} else { 
					clickX = interacts[sceneIndex].cx;
					clickY = interacts[sceneIndex].cy;
				}
			
				// Compute goal coordinates
				test = IntersectSegmentPolygon(unitX, unitY, clickX, clickY, MAX_POLYGON, polygonX, polygonY, &interX, &interY);
				if (test && (unitX != interX || unitY != interY)) { 	// Check that we are not stuck on a polygon segment
					goalX = interX;
					goalY = interY;
				} else {
					// Move directly to mouse cursor (if in allowed area, and not crossing other parts of polygon)
					if (test < 2 && PointInsidePolygon(clickX, clickY, MAX_POLYGON, polygonX, polygonY)) {
						goalX = clickX;
						goalY = clickY;
					}
				}
				
				// Save index, for interaction when we reach position
				sceneInteract = sceneIndex;
			}
		}
				
		// Process unit motion
		if (clock()>gameClock+unitTicks) {
			gameClock = clock();
			deltaX = goalX - unitX;
			deltaY = goalY - unitY;
			if (deltaX || deltaY) {
				// Move in steps of 3 max.
				unitX += SIGN(deltaX) * MIN(ABS(deltaX), unitStep); 
				unitY += SIGN(deltaY) * MIN(ABS(deltaY), unitStep);
				if (unitX > 320) unitX = 0;
				if (unitY > 200) unitY = 0;
				
				// Update frame number
				if (deltaX > 0) {
					if (unitFrame < frameWalkRightBeg) unitFrame = frameWalkRightBeg;
					unitFrame += 1; if (unitFrame > frameWalkRightEnd) unitFrame = frameWalkRightBeg;
					waitFrame = frameWaitRight;
				} else {
					if (unitFrame > frameWalkLeftEnd) { unitFrame = frameWalkLeftBeg; }
					unitFrame += 1; if (unitFrame > frameWalkLeftEnd) unitFrame = frameWalkLeftBeg;
					waitFrame = frameWaitLeft;
				}
			} else {
				// Process trigger (if any) and set wait frame
				if (sceneInteract != 255) {
					ProcessInteract(sceneInteract, sceneItem, unitX, unitY);
					sceneInteract = 255;
					sceneItem = 255;
				}
				unitFrame = waitFrame;
			}
		}
	
		// Set sprites
		LocateSprite(mouseX+4, mouseY+4);
		SetSprite(0, mouseL);
	#if (defined __CBM__) 
		LocateSprite(unitX, unitY-20);
	#elif (defined __ATARI__)
		LocateSprite(unitX, unitY-16);
	#else
		LocateSprite(unitX, unitY-10);
	#endif
	#if (defined __ATARI__) || (defined __CBM__)
		SetSprite(1, unitFrame);		// Unit color #1
		SetSprite(2, unitFrame+14);	// Unit color #2
		SetSprite(3, unitFrame+28);	// Unit color #3
		SetSprite(4, unitFrame+42);	// Unit color #4
	#else
		SetSprite(1, unitFrame);
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
	CropSprite(0,8); // Mouse cursor only occupies third of frame
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
	GameLoop();
	
	// Black-out screen and clear key
	DisableSprite(-1);	// "-1" disables all sprites
	ExitBitmapMode();
	StopSFX();
	
    // Done
    return EXIT_SUCCESS;	
}
