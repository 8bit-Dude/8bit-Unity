
#include "definitions.h"

#if (defined __NES__)
  #pragma rodata-name("BANK0")
  #pragma code-name("BANK0")  	
  #pragma bss-name(push, "XRAM")
#endif

// Mesh list properties
unsigned char nMesh, iMesh, nFace, nVert;
unsigned char names[MAXMESH][8], nVerts[MAXMESH], nFaces[MAXMESH];
fix8 trsf[MAXMESH][9];

// Raw mesh data
unsigned char faces[MAXFACE][3]; 
fix8 verts[MAXVERT][3];

// Transformed mesh data 
fix8 trans[MAXVERT][3];
fix8 norms[MAXFACE][3];

// Rasterized mesh data
unsigned char pxls[MAXVERT][2];

#if (defined __NES__)
 #pragma bss-name(pop)
#endif

// Screen properties
#if defined (__APPLE2__)
  #define SCREENY 8u
  #define SCREENW 105u
  #define SCREENH 184u
#elif defined (__ATARI__) || defined(__CBM__)
  #define SCREENY 8u
  #define SCREENW 120u
  #define SCREENH 192u
#elif defined(__LYNX__)
  #define SCREENY 6u
  #define SCREENW 120u
  #define SCREENH 96u
#elif defined (__NES__)
  #define SCREENY 1u
  #define SCREENW 46u
  #define SCREENH 46u
#elif defined (__ORIC__)
  #define SCREENY 8u
  #define SCREENW 174u
  #define SCREENH 192u
#endif
fix8 canvasW = Fix8(2);
fix8 canvasH = Fix8(2);

// Camera properties
fix8 camRotX = 60, camRotZ = 140, camZoom = 128;
fix8 camVec[3] = {0,0,256};
fix8 worldToCamera[12] = { 256,     0,     0,
							 0,   256,     0,
							 0,     0,   256,
							 0,     0, 10240, };

void UpdateCamera(void)
{
	fix8 xCos, zCos;
	fix8 xSin, zSin;
	
	// Compute cos and sin of angles
	xCos = _cos(camRotX); xSin = _sin(camRotX);
	zCos = _cos(camRotZ); zSin = _sin(camRotZ);
	
	// Update camera matrix
	worldToCamera[0+0] = imul16x16r32( zCos,256u) / camZoom;
	worldToCamera[0+1] = imul16x16r32(-zSin,xCos) / camZoom;
	worldToCamera[0+2] = imul16x16r32( zSin,xSin) / camZoom;
	worldToCamera[3+0] = imul16x16r32( zSin,256u) / camZoom;
	worldToCamera[3+1] = imul16x16r32( zCos,xCos) / camZoom;
	worldToCamera[3+2] = imul16x16r32(-zCos,xSin) / camZoom;
	worldToCamera[6+0] = (      	  0	    	  );
	worldToCamera[6+1] = imul16x16r32( xSin,256u) / camZoom;
	worldToCamera[6+2] = imul16x16r32( xCos,256u) / camZoom;
	
	// Update camera vector
	camVec[0] = imul16x16r32( zSin,-xSin) / 256u;
	camVec[1] = imul16x16r32(-zCos,-xSin) / 256u;
	camVec[2] = -xCos;	
}			 
							 
fix8 V3dotV3(fix8 *v1, fix8 *v2)
{
	return (imul16x16r32(v1[0],v2[0]) + imul16x16r32(v1[1],v2[1]) + imul16x16r32(v1[2],v2[2]))/256;
}

void M43multV3(fix8 *mat, fix8 *in, fix8 *out)
{
	out[0] = (imul16x16r32(in[0],mat[0+0]) + imul16x16r32(in[1],mat[3+0]) + imul16x16r32(in[2],mat[6+0]) + imul16x16r32(256,mat[9+0])) / 256u;
	out[1] = (imul16x16r32(in[0],mat[0+1]) + imul16x16r32(in[1],mat[3+1]) + imul16x16r32(in[2],mat[6+1]) + imul16x16r32(256,mat[9+1])) / 256u;
	out[2] = (imul16x16r32(in[0],mat[0+2]) + imul16x16r32(in[1],mat[3+2]) + imul16x16r32(in[2],mat[6+2]) + imul16x16r32(256,mat[9+2])) / 256u;
}

void Transform(unsigned char index) 
{
	unsigned int i, beg, end;
	fix8 uX,uY,uZ,vX,vY,vZ;
	fix8 cosA,sinA,cosB,sinB,cosC,sinC;	
	fix8 *vertex, *trans0, *trans1, *trans2;	
	fix8 *position, *rotation, *dimension;
	signed long x,y,z;
	
	// Compute vertex range
	if (index > 0)
		beg = nVerts[index-1];
	else
		beg = 0;
	end = nVerts[index];

	// Get pointers to transform components
	position = &trsf[index][0];
	rotation = &trsf[index][3];
	dimension = &trsf[index][6];
	
	// Compute cos/sin of rotation angles
	cosA = _cos(rotation[0]); sinA = _sin(rotation[0]);
	cosB = _cos(rotation[1]); sinB = _sin(rotation[1]);
	cosC = _cos(rotation[2]); sinC = _sin(rotation[2]);	

	// Loop through vertices
	for (i=beg; i<end; ++i) {
		// Get vertex/transform pointer
		vertex = verts[i];
		trans0 = trans[i];
		
		// Read point from buffer and rescale
		x = imul16x16r32(vertex[0],dimension[0])/256u; 
		y = imul16x16r32(vertex[1],dimension[1])/256u; 
		z = imul16x16r32(vertex[2],dimension[2])/256u; 

		// Write the rotated/translated point
		trans0[0] = (x*cosC*cosB - y*sinC*cosA + y*(cosC*sinB*sinA/256u) + z*sinC*sinA + z*(cosC*sinB*cosA/256u)) / 65536u + position[0];
		trans0[1] = (x*sinC*cosB + y*cosC*cosA + y*(sinC*sinB*sinA/256u) - z*cosC*sinA + z*(sinC*sinB*cosA/256u)) / 65536u + position[1];
		trans0[2] = (-x*sinB*256 + y*cosB*sinA + z*cosB*cosA) / 65536u + position[2];		
	}
		
	// Compute face range
	if (index > 0)
		beg = nFaces[index-1];
	else
		beg = 0;
	end = nFaces[index];		

	// Compute normals
	for (i=beg; i<end; ++i) {
		// Get vertex transforms
		trans0 = trans[faces[i][0]];
		trans1 = trans[faces[i][1]];
		trans2 = trans[faces[i][2]];
		
		// Compute face vectors
		uX = trans1[0] - trans0[0];
		uY = trans1[1] - trans0[1];
		uZ = trans1[2] - trans0[2];
		vX = trans2[0] - trans0[0];
		vY = trans2[1] - trans0[1];
		vZ = trans2[2] - trans0[2];
		
		// Compute normal from cross product
		norms[i][0] = (imul16x16r32(uY,vZ) - imul16x16r32(uZ,vY))/256u;
		norms[i][1] = (imul16x16r32(uZ,vX) - imul16x16r32(uX,vZ))/256u;
		norms[i][2] = (imul16x16r32(uX,vY) - imul16x16r32(uY,vX))/256u;
		
		// Normalize (TODO: Buggy)
		//norm = sqrt(x*x + y*y + z*z);
		//WriteFix8(norms,i*3+0, (x*256)/norm);
		//WriteFix8(norms,i*3+1, (y*256)/norm);
		//WriteFix8(norms,i*3+2, (z*256)/norm);
	}	
}

void Rasterize(unsigned char index) 
{
	unsigned int i, beg, end;
	signed int x, y;
	fix8 camPt[3];
	
	// Compute vertex range
	if (index > 0)
		beg = nVerts[index-1];
	else
		beg = 0;
	end = nVerts[index];

	// Loop through vertices
	for (i=beg; i<end; ++i) {
		// Project point to screen
		M43multV3(worldToCamera, trans[i], camPt);
		x = imul16x16r32(SCREENW, (canvasW/2u - imul16x16r32(256,camPt[0])/camPt[2])) / canvasW;
		y = SCREENH - imul16x16r32(SCREENH, (canvasH/2 - imul16x16r32(256,camPt[1])/camPt[2])) / canvasH;
		if (x<0 || x>=SCREENW || y<0 || y>=SCREENW) {
			pxls[i][0] = 255;
		} else {
			pxls[i][0] = x;
			pxls[i][1] = y+SCREENY;
		}
	}
}

void RasterizeAll(void)
{
	unsigned char m;

	// Loop through meshes
	for (m=0; m<nMesh; m++) {
		Rasterize(m);
	}	
}

void DrawLine(unsigned char *p0, unsigned char *p1)
{
	unsigned char i;
	signed ux, uy;
	
	ux = p1[0] - p0[0];	
	uy = p1[1] - p0[1];
	
	for (i=0; i<9; i++) {
		pixelX = p0[0] + imul8x8r16(i,ux)/8u; 
		pixelY = p0[1] + imul8x8r16(i,uy)/8u; 
		SetPixel(WHITE);
	}
}

void RenderAll(void) 
{
	unsigned int i, beg, end;
	unsigned char m, *v;
	
	// Clear Canvas
	txtX = 0; txtY = 1;
	inkColor = WHITE; paperColor = BLACK;
	PrintBlanks(TXT_COLS-10, TXT_ROWS-1);

	// Loop through meshes
	for (m=0; m<nMesh; m++) {
		// Compute face range
		if (m > 0)
			beg = nFaces[m-1];
		else
			beg = 0;
		end = nFaces[m];
		
		// Do Draw?
		for (i=beg; i<end; ++i) {
			// Check all pixels are within screen
			v = faces[i];
			if (pxls[v[0]][0] == 255 || pxls[v[0]][0] == 255 || pxls[v[0]][0] == 255)
				continue;
			
			// Check normals against camera vector
			if (V3dotV3(norms[i], camVec) <= -16)
				continue;
						
			// Draw the triangle sides
			DrawLine(pxls[v[0]], pxls[v[1]]);
			DrawLine(pxls[v[1]], pxls[v[2]]);
			DrawLine(pxls[v[2]], pxls[v[0]]);
		}
	}
}
