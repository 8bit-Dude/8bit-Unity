
#include "definitions.h"

fix8 vertBox[8][3]  = {{-128,-128,-128},{-128,128,-128},{128,128,-128},{128,-128,-128},
					   {-128,-128, 128},{-128,128, 128},{128,128, 128},{128,-128, 128}};
char faceBox[12][3] = {{0,1,2},{0,2,3},{0,5,1},{0,4,5}, 
					   {4,6,5},{4,7,6},{3,2,6},{3,6,7}, 
					   {3,4,0},{3,7,4},{1,5,2},{2,5,6}};	

fix8 vertCon[9][3]  = {{128,0,-128},{91,91,-128},{0,128,-128},{-91,91,-128},{-128,0,-128},{-91,-91,-128},{0,-128,-128},{91,-91,-128},{0,0,128}};
char faceCon[14][3] = {{0,1,8},{1,2,8},{2,3,8},{3,4,8},{4,5,8},{5,6,8},{6,7,8},{7,0,8},
					   {0,4,1},{1,4,3},{1,3,2},{5,4,0},{7,5,0},{6,5,7}};
			  
fix8 vertCyl[16][3] = {{128,0,-128},{91,91,-128},{0,128,-128},{-91,91,-128},{-128,0,-128},{-91,-91,-128},{0,-128,-128},{91,-91,-128},
					   {128,0, 128},{91,91, 128},{0,128, 128},{-91,91, 128},{-128,0, 128},{-91,-91, 128},{0,-128, 128},{91,-91, 128}};
char faceCyl[28][3] = {{1,8,0},{9,8,1},{2,9,1},{10,9,2},{3,10,2},{11,10,3},{4,11,3},{12,11,4},
							  {5,12,4},{13,12,5},{6,13,5},{14,13,6},{7,14,6},{15,14,7},{0,15,7},{8,15,0},
							  {0,4,1},{1,4,3},{1,3,2},{5,4,0},{7,5,0},{6,5,7},
							  {9,12,8},{11,12,9},{11,10,9},{8,12,13},{8,13,15},{15,13,14}};	

fix8 vertSph[12][3] = {{-67,0,109},{ 67,0,109},{-67,0,-109},{ 67,0,-109},
					   {0,109, 67},{0,109,-67},{0,-109, 67},{0,-109,-67},
					   {109, 67,0},{-109,67,0},{ 109,-67,0},{-109,-67,0}};
char faceSph[20][3] = {{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
					   {8,10,1},{8,3,10},{5,3,8}, {5,2,3}, {2,7,3},
					   {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
					   {6,1,10},{9,0,11},{9,11,2},{9,2,5}, {7,2,11}};

fix8 vertTor[16][3] = {{64,0,0},{96,0,32},{128,0,0},{96,0,-32},
					   {0,64,0},{0,96,32},{0,128,0},{0,96,-32},
					   {-64,0,0},{-96,0,32},{-128,0,0},{-96,0,-32},
					   {0,-64,0},{0,-96,32},{0,-128,0},{0,-96,-32}};
char faceTor[32][3] = {{0,1,4}, {5,1,4},{1,2,5},{6,2,5},{2,3,6},{7,3,6},{3,4,7},{0,4,7},
					   {4,5,8}, {9,5,8},{5,6,9},{10,6,9},{6,7,10},{11,7,10},{7,8,11},{4,8,11},
					   {8,9,12},{13,9,12},{9,10,13},{14,10,13},{10,11,14},{15,11,14},{11,12,15},{8,12,15},
					   {12,13,0},{1,13,0},{13,14,1},{1,14,1},{14,15,2},{3,15,2},{15,0,3},{12,0,3}};
					   
char** namePrim[] = { "  BOX  ", " CONE  ", "CYLINDE", "SPHERE ", " TORUS " };
fix8** vertPrim[] = {  vertBox ,  vertCon ,  vertCyl ,  vertSph ,  vertTor  };
char** facePrim[] = {  faceBox ,  faceCon ,  faceCyl ,  faceSph ,  faceTor  };
char   vertCoun[] = {     8    ,     9    ,     16   ,     12   ,     16    };
char   faceCoun[] = {     12   ,     14   ,     28   ,     20   ,     32    };

void Push(unsigned char prim)
{	
	unsigned char i, fn, vn;
	
	// Fetch vertice/face count
	vn = vertCoun[prim];
	fn = faceCoun[prim];
	
	// Copy name/faces/vertices
	memcpy(names[nMesh], namePrim[prim], 8);
	memcpy(verts[nVert], vertPrim[prim], 6*vn); // 3 * fix8
	memcpy(faces[nFace], facePrim[prim], 3*fn); // 3 * char
	
	// Shift face numbers
	for (i=nFace; i<nFace+fn; i++) {
		faces[i][0] += nVert;
		faces[i][1] += nVert;
		faces[i][2] += nVert;
	}
	
	// Update faces/vertices pointers
	nVert += vn; nFace += fn;
	nVerts[nMesh] = nVert;
	nFaces[nMesh] = nFace;
	iMesh = nMesh;
	nMesh++;
} 

void Pop(void)
{
	unsigned char i, minF, minV, dropF, dropV, shiftF, shiftV, shiftN;
	
	// Check if any work is needed?
	if (!nMesh) return;

	// Compute number of dropped vertex/face
	if (iMesh) {
		dropV = nVerts[iMesh]-nVerts[iMesh-1];
		dropF = nFaces[iMesh]-nFaces[iMesh-1];	
		minV = nVerts[iMesh-1];		
		minF = nFaces[iMesh-1];	
	} else {
		dropV = nVerts[0];
		dropF = nFaces[0];	
		minV = 0;		
		minF = 0;		
	}
	
	// Was it the last element?
	if (iMesh != nMesh-1) {
		// Compute number of shifted vertex/face
		shiftV = nVerts[nMesh-1]-nVerts[iMesh];
		shiftF = nFaces[nMesh-1]-nFaces[iMesh];	
		shiftN = (nMesh-1-iMesh);

		// Shift mesh data
		memcpy(verts[minV], verts[minV+dropV], 6*shiftV);
		memcpy(trans[minV], trans[minV+dropV], 6*shiftV);
		memcpy(faces[minF], faces[minF+dropF], 3*shiftF);
		memcpy(norms[minF], norms[minF+dropF], 6*shiftF);
		memcpy(names[iMesh], names[iMesh+1], 8*shiftN);
		memcpy(trsf[iMesh], trsf[iMesh+1], 18*shiftN);
		
		// Shift vertex/face counters
		for (i=iMesh; i<nMesh-1; i++) {
			nVerts[i] = nVerts[i+1] - dropV;
			nFaces[i] = nFaces[i+1] - dropF;
		}		
		
		// Shift face indices
		for (i=minF; i<(minF+shiftF); i++) {
			faces[i][0] -= dropF;
			faces[i][1] -= dropF;
			faces[i][2] -= dropF;
		}	
	}
	
	// Update counters
	nVert -= dropV;
	nFace -= dropF;
	if (iMesh) iMesh--;
	nMesh--;
}
