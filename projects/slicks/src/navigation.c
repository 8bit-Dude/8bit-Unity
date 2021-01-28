
#include "definitions.h"

// See game.c
extern unsigned char gameLineUp[4];

// Navigation variables
Vehicle cars[MAX_PLAYERS];
Waypoint ways[MAX_WAYPOINTS];
Ramp ramps[MAX_RAMPS];
unsigned char numWays;
unsigned char numRamps;

// Lineup positions
unsigned int lineupX[MAX_PLAYERS];
unsigned int lineupY[MAX_PLAYERS];
unsigned int lineupAng[MAX_PLAYERS];

#if defined __LYNX__
  unsigned char *buffer = (unsigned char*)SHAREDRAM;
#else
  unsigned char buffer[128];
#endif

// Function to load *.nav files
void LoadNavigation(char *filename)
{
	// Read nav file contents
	unsigned char n,*p;
  #if defined __APPLE2__
	if (FileOpen(filename)) {
		FileRead(buffer, 128);
		FileClose();
	}
  #elif defined __ATARI__
	FileOpen(filename);
	FileRead(buffer, 128);
  #elif defined __ORIC__
	FileRead(filename, buffer);
  #elif defined __LYNX__
	FileRead(filename);
  #else
	FILE* fp = fopen(filename, "rb");
	fread(buffer, 1, 128, fp);
	fclose(fp);
  #endif
  
	// Read Lineup
	p = buffer;
	n = *p*2; p++;
	memcpy(lineupX, p, n); p += n;
	memcpy(lineupY, p, n); p += n;
	memcpy(lineupAng, p, n); p += n;
	
	// Read Navigation Cylinders
	numWays = *p; p++;
	memcpy(&ways[0].x, p, numWays*8); p += numWays*8;
	
	// Read Jump Ramps
	numRamps = *p; p++;
	memcpy(&ramps[0].x[0], p, numRamps*8);
	
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
	for (i=0; i<MAX_PLAYERS; ++i) {
		j = gameLineUp[i];
		cars[i].x2 =   lineupX[j]; 
		cars[i].y2 =   lineupY[j];
		cars[i].ang1 = lineupAng[j];
		cars[i].ang2 = lineupAng[j];
		cars[i].vel = 0;		
        cars[i].way = 0;
        cars[i].lap = -1;
        cars[i].joy = 0;             
	}
}

// Function to check ramp logics
Ramp *ramp;
char CheckRamps(Vehicle *car)
{
    // Gotta check them all...
    unsigned char i;
	for (i=0; i<numRamps; ++i) {
        ramp = &ramps[i];
        if (car->x2 < ramp->x[0] && car->x2 < ramp->x[1]) { continue; }
        if (car->x2 > ramp->x[0] && car->x2 > ramp->x[1]) { continue; }
        if (car->y2 < ramp->y[0] && car->y2 < ramp->y[1]) { continue; }
        if (car->y2 > ramp->y[0] && car->y2 > ramp->y[1]) { continue; }
        return 1;
    }
    return 0;
}

// Functions to check navigation around cylinders
signed char v1[2], v2[2], v90[2];
unsigned char step;
Waypoint *way;

void GetWaypoint(Vehicle *car)
{
	// Prepare waypoint variables
	way = &ways[car->way/2u];
	step = car->way%2;
}

char CheckWaypoint(Vehicle *car)
{
	// Fetch waypoint
	GetWaypoint(car);
	
	// Check waypoint vector dot products against old position
	v1[0] = (car->x1 - way->x)/16u;
	v1[1] = (car->y1 - way->y)/16u;		
	if ( (dot(v1, way->v[step]) >= 0) ) {

		// Check waypoint vector dot products against new position
		v2[0] = (car->x2 - way->x)/16u;
		v2[1] = (car->y2 - way->y)/16u;				
		if ( (dot(v2, way->v[step]) >= 0) ) {
			
			// Compute 90 deg rotated vector
			v90[0] = -way->v[step][1];
			v90[1] =  way->v[step][0];

			// Check dot products with 90 deg rotated vector
			if ((dot(v1, v90) >= 0) && (dot(v2, v90) <= 0)) { 
			#ifdef DEBUG_NAV
				PrintBlanks(0, 0, 2, 0);
				PrintNum(0, 0, car->way+1);
			#endif
				return 1; 
			}
			if ((dot(v1, v90) <= 0) && (dot(v2, v90) >= 0)) { 
			#ifdef DEBUG_NAV
				PrintBlanks(0, 0, 2, 0);
				PrintNum(0, 0, car->way+1);
			#endif
				return 1; 
			}
		}
	}
	return 0;
}

int GetWaypointDistance(Vehicle *car)
{
	signed char dx, dy;	
	GetWaypoint(car);
	dx = (car->x2 - way->x)/16u;
	dy = (car->y2 - way->y)/16u;	
	return ABS(dx)+ABS(dy);
}

int GetWaypointAngle(Vehicle *car)
{
	unsigned char dx = 128, dy = 128;	
	GetWaypoint(car);
	dx += (way->x + 8*way->v[step][0] - car->x2)/16u;
	dy -= (way->y + 8*way->v[step][1] - car->y2)/16u;
	return (45*(unsigned int)atan2(dy,dx))/32u;
}
