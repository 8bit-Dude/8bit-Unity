
#include "unity.h"

// Sprite definitions
#define spriteFrames 14
#if defined __APPLE2__	
	#define spriteCols   7
	#define spriteRows   10
	unsigned char spriteColors[] = { };	//  Colors are pre-assigned in the sprite sheet
#elif defined __ATARI__
	#define spriteCols   8
	#define spriteRows   10
	unsigned char spriteColors[] = { 0x0e, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0xea, 0xea };  // Refer to atari palette in docs
#elif defined __ATMOS__
	#define spriteCols   12
	#define spriteRows   10
	unsigned char spriteColors[] = { YELLOW, LBLUE, LBLUE, LBLUE, LBLUE, LBLUE, RED, LGREEN };	 	// Number of sprites unlimited in theory...
#elif defined __CBM__
	#define spriteCols   12
	#define spriteRows   21
	unsigned char spriteColors[] = { WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, YELLOW, BLACK };	// 0-8: Sprite colors, 9-10: Shared colors
#elif defined __LYNX__
	#define spriteCols   9
	#define spriteRows   13
	unsigned char spriteColors[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,   // Default palette
									 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }; // Default palette	
#endif

// Animation definitions
#define frameWaitLeft     2
#define frameWalkLeftBeg  3
#define frameWalkLeftEnd  7
#define frameWaitRight    8
#define frameWalkRightBeg 9
#define frameWalkRightEnd 13

// Scene definitions (referenced against 320 x 200 image)
unsigned char polygonN = 19;
unsigned int polygonX[19] = {   0,  53, 120, 138,  65,  24, 126, 115, 184, 270, 282, 251, 232, 210, 229, 320, 320,   0,   0 };
unsigned int polygonY[19] = { 138, 162, 169, 144, 138, 126, 103,  90,  78, 102, 120, 137, 124, 143, 168, 116, 200, 200, 138 };

unsigned char sceneN = 6;
unsigned int sceneX[6] = { 55, 222, 260, 230, 69, 32 };
unsigned int sceneY[6] = { 81,  66,  70, 134, 60, 77 };
unsigned int sceneR[6] = { 10,  10,   5,   7,  5,  5 };
unsigned char *sceneLbl[6] = { NULL, NULL, NULL, NULL, NULL, NULL }; 
unsigned char *sceneQst[6] = { NULL, NULL, NULL, NULL, NULL, NULL }; 
unsigned char *sceneAns[6] = { NULL, NULL, NULL, NULL, NULL, NULL }; 

unsigned char lbl0[] = "Notable";
unsigned char lbl1[] = "Old Men";
unsigned char lbl2[] = "Bottle";
unsigned char lbl3[] = "Flower";
unsigned char lbl4[] = "Sausage";
unsigned char lbl5[] = "Switch";
unsigned char qst0[] = "Dear sir, you look powerful. Could\nyou please help us in this quest?\0";
unsigned char qst1[] = "Hey, villagers, do you know\nthe house of the lord Tazaar?\0";
unsigned char ans1[] = "It's at the end of the village,\nthe house with the fountain...\0";

unsigned char IntersectSegments(signed long s1x1, signed long s1y1, signed long s1x2, signed long s1y2,
							    signed long s2x1, signed long s2y1, signed long s2x2, signed long s2y2,
								signed int *ix,   signed int *iy)
{
    signed long denom, numer1, numer2, ratio1, ratio2;
	
	denom = ((s1x2 - s1x1) * (s2y2 - s2y1)) - ((s1y2 - s1y1) * (s2x2 - s2x1));

    // Segments are parallel 
    if (denom == 0) return 0;

    numer1 = ((s1y1 - s2y1) * (s2x2 - s2x1)) - ((s1x1 - s2x1) * (s2y2 - s2y1));
    numer2 = ((s1y1 - s2y1) * (s1x2 - s1x1)) - ((s1x1 - s2x1) * (s1y2 - s1y1));

    ratio1 = (100*numer1) / denom;
    ratio2 = (100*numer2) / denom;

    if ((ratio1 < 0 || ratio1 > 100) || (ratio2 < 0 || ratio2 > 100)) return 0;

    // Find intersection point
    *ix = s1x1 + (ratio1 * (s1x2 - s1x1))/100;
    *iy = s1y1 + (ratio1 * (s1y2 - s1y1))/100;
    return 1;
}

unsigned char IntersectPolygon(signed long s1x1, signed long s1y1, signed long s1x2, signed long s1y2, signed int *ix, signed int *iy)
{
	unsigned char i;
	signed int tmpX, tmpY; 
	signed long tmpD, dist = 999999;
	
	// Find intersection point with polygon that is nearest to segment point 1
	for (i=1; i<polygonN; i++) {
		if (IntersectSegments(s1x1, s1y1, s1x2, s1y2, polygonX[i-1], polygonY[i-1], polygonX[i], polygonY[i], &tmpX, &tmpY)) {
			tmpD = (s1x1 - tmpX) * (s1x1 - tmpX) + (s1y1 - tmpY) * (s1y1 - tmpY);
			if (tmpD < dist) {
				*ix = tmpX;
				*iy = tmpY;
				dist = tmpD;
			}
		}
	}
	if (dist < 999999) { return 1; } else { return 0; }
}

unsigned char SearchScene(signed int searchX, signed int searchY) 
{
	unsigned char i;
	for (i=0; i<sceneN; i++) {
		if (((searchX - sceneX[i]) * (searchX - sceneX[i]) + 
		     (searchY - sceneY[i]) * (searchY - sceneY[i])  ) < (sceneR[i]*sceneR[i])) {
			return i;
		}
	}
	return 255;
}

void PrintMessage(unsigned char *msg)
{
	unsigned char i = 0, row = CHR_ROWS-2, col = 1;
	
	// Reset panel
	DrawPanel(1, CHR_ROWS-2, CHR_COLS-1, CHR_ROWS-1);
	
	// Print message across two lines, by taking into account line feed '\n'
	while (msg[i] != '\0') {
		if (msg[i] == '\n') {
			++row;
			col = 1;
		} else {
			PrintChr(++col, row, GetChr(msg[i]));
		}
		++i;
	}
}

void InitScene()
{
	// Assign scene labels and messages
	sceneLbl[0] = lbl0;
	sceneLbl[1] = lbl1;
	sceneLbl[2] = lbl2;
	sceneLbl[3] = lbl3;
	sceneLbl[4] = lbl4;
	sceneLbl[5] = lbl5;
	sceneQst[0] = qst0;
	sceneQst[1] = qst1;
	sceneAns[1] = ans1;

	// Assign ink/paper colors
	inkColor = WHITE;
	paperColor = ORANGE;
}

void RunScene(void)
{
	unsigned char joy, sceneIndex, sceneLabel, sceneQestion;
	unsigned char mouseAction = 0, mouseFrame = 0, mouseMotion = 0;
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
		if (!(joy & JOY_BTN1) || !(joy & JOY_BTN2))  { mouseFrame = 1; } else { mouseFrame = 0; mouseAction = 0; }
		
		// Search scene
		if (mouseMotion) {
			sceneIndex = SearchScene(mouseX, mouseY);			
			if (sceneLabel != sceneIndex) {
				if (sceneIndex == 255) {
					DrawPanel(1, CHR_ROWS-2, CHR_COLS-1, CHR_ROWS-1);
				} else {
					PrintStr(1, CHR_ROWS-2, sceneLbl[sceneIndex]);
				}
				sceneLabel = sceneIndex;
			}
		}
		
		// Process commands
		if (mouseFrame && !mouseAction) {
			// Compute goal position
			if (IntersectPolygon(unit1X, unit1Y, mouseX, mouseY, &interX, &interY)) {
				goalX = interX;
				goalY = interY;
			} else {
				goalX = mouseX;
				goalY = mouseY;
			}
			// Look for scene object under the mouse
			sceneIndex = SearchScene(mouseX, mouseY);	
			if (sceneIndex != 255 && sceneQst[sceneIndex] != NULL) {
				PrintMessage(sceneQst[sceneIndex]);
			}
			BumpSFX();
			mouseAction = 1;
		}
				
		// Process unit motion
		if (clock()>gameClock+5) {
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
				unit1Frame = waitFrame;
			}
			gameClock = clock();
		}
	
		// Set sprites
		LocateSprite(mouseX+4, mouseY+4);
		SetSprite(0, mouseFrame);
		LocateSprite(unit1X, unit1Y-10);
		SetSprite(1, unit1Frame);
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

	// Init and Run Scene
	InitScene();
	RunScene();
	
	// Black-out screen and clear key
	DisableSprite(-1);	// "-1" disables all sprites
	ExitBitmapMode();
	StopSFX();
	
    // Done
    return EXIT_SUCCESS;	
}
