
#include "definitions.h"

// See game.c
extern unsigned char gameLineUp[4];

#if defined __NES__
 #pragma bss-name(push, "XRAM")
#endif

// Lineup positions
unsigned int lineupX[MAX_PLAYERS];
unsigned int lineupY[MAX_PLAYERS];
unsigned int lineupAng[MAX_PLAYERS];

// Waypoints and ramps
unsigned char numWays;
Waypoint ways[MAX_WAYPOINTS];
unsigned char numRamps;
Ramp ramps[MAX_RAMPS];

// Navigation variables
Vehicle cars[MAX_PLAYERS];
signed char *vWay;
Waypoint *way;

#if defined __NES__
 #pragma bss-name(pop) 
#endif

// See game.c
extern Vehicle *iCar;
extern int iX, iY;

// Function to load *.nav files
void LoadNavigation(char *filename)
{
	// Read nav file contents
	unsigned char n,*p;
  #if defined __APPLE2__
	if (FileOpen(filename)) {
		FileRead(lineupX, 114);
		FileClose();
	}
  #elif defined __ATARI__
	if (FileOpen(filename)) {
		FileRead(lineupX, 114);
	}
  #elif defined __LYNX__
	FileRead(filename);
	memcpy(lineupX, (unsigned char*)SHAREDRAM, 114);
	
  #elif defined(__NES__) || defined(__ORIC__)
	FileRead(filename, lineupX);
	
  #elif defined __CBM__
	FILE* fp = fopen(filename, "rb");
	fread(lineupX, 1, 114, fp);
	fclose(fp);
  #endif

#ifdef DEBUG_NAV
	for (n=0; n<numWays; ++n) {
		// Display waypoints (debugging)
		LocatePixel(ways[n].x/8u, ways[n].y/8u); SetPixel(BLACK);
		LocatePixel(ways[n].x/8u+ways[n].v[0][0], ways[n].y/8u+ways[n].v[0][1]); SetPixel(BLACK); 
		LocatePixel(ways[n].x/8u+ways[n].v[1][0], ways[n].y/8u+ways[n].v[1][1]); SetPixel(BLACK);         
	}
#endif
}

// Reset cars to Line-up positions 
void ResetLineUp()
{
	unsigned char i,j;
	Vehicle *car;
	for (i=0; i<MAX_PLAYERS; ++i) {
		j = gameLineUp[i];
		car = &cars[i];
		car->x2 =   lineupX[j]; 
		car->y2 =   lineupY[j];
		car->ang1 = lineupAng[j];
		car->ang2 = lineupAng[j];
		bzero(&car->vel, 4);		
        car->lap = -1;
	}
}

// Functions to check navigation around cylinders
signed char v1[2], v2[2], dist[2], cross[2];

char CheckWaypoint(void)
{
	signed int dotCross1, dotCross2;
	
	// Check waypoint vector dot products against old position
	v1[0] = (iCar->x1 - way->x)/32u;
	v1[1] = (iCar->y1 - way->y)/32u;	
	if ( (dot(v1, vWay) >= 0) ) {
		
		// Check waypoint vector dot products against new position
		v2[0] = (iX - way->x)/32u;
		v2[1] = (iY - way->y)/32u;				
		if ( (dot(v2, vWay) >= 0) ) {
			
			// Check proximty to waypoint (except for finish line!)
			if (iCar->way) {
				dist[0] = v2[0] - vWay[0]/4u;
				dist[1] = v2[1] - vWay[1]/4u;
				if (dot(dist, dist) < 24)
					return 1;
			}
		
			// Compute dot products with 90 deg rotated vector
			cross[0] = -vWay[1];
			cross[1] =  vWay[0];
			dotCross1 = dot(v1, cross);
			dotCross2 = dot(v2, cross);

			// Check dot products with 90 deg rotated vector
			if (dotCross1 >= 0 && dotCross2 <= 0) { 
			#ifdef DEBUG_NAV
				txtX = 0; txtY = 0; PrintBlanks(2, 0); PrintNum(iCar->way+1);
			#endif
				return 1; 
			}
			if (dotCross1 <= 0 && dotCross2 >= 0) { 
			#ifdef DEBUG_NAV
				txtX = 0; txtY = 0; PrintBlanks(2, 0); PrintNum(iCar->way+1);
			#endif
				return 1; 
			}
		}
	}
	return 0;
}

const signed char tan[19] = {91,30,17,11,8,6,4,2,1,-1,-2,-4,-6,-8,-11,-17,-30,-91,-128};

int GetWaypointAngle(unsigned char i) 
{
	unsigned char j = 0;
	signed char dy, tn;
	signed int dx;
	
	// Compute vector to waypoint
	dx =  (way->x + 8*vWay[0] - iX);
	dy = -(way->y + 8*vWay[1] - iY)/8u;
	dx += (16*i-32); dy += (2*i-4); // Shift target (to allow overtaking)
	
	// Fast atan2 routine with 36 x 10 degs increments
	if (dy == 0) {
		 // Select value directly (Cannot divide by 0)
		if (dx >= 0)
			return 0;
		else
			return 180;
	} else {
		// Use simple division and check range
		dx = dx/dy;
		if (dx < -128)
			tn = -128;
		else 
		if (dx > 127)
			tn = 127;
		else
			tn = dx;
			
		// Check against reference table
		while (tn < tan[j])
			j++;
		
		// Compute angle
		dx = j*10;
		if (dy < 0)
			dx += 180;
		return dx;
	}
}

void GetWaypoint(void)
{
	// Prepare waypoint variables
	way = &ways[iCar->way/2u];
	vWay = way->v[iCar->way&1];
}

// Function to check ramp logics
Ramp *ramp;
char CheckRamps(void)
{
    // Gotta check them all...
    unsigned char i;
	for (i=0; i<numRamps; ++i) {
        ramp = &ramps[i];
        if (iX < ramp->x[0] && iX < ramp->x[1]) { continue; }
        if (iX > ramp->x[0] && iX > ramp->x[1]) { continue; }
        if (iY < ramp->y[0] && iY < ramp->y[1]) { continue; }
        if (iY > ramp->y[0] && iY > ramp->y[1]) { continue; }
        return 1;
    }
    return 0;
}