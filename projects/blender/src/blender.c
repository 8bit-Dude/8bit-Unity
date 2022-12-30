
#include "definitions.h"

callback *callRL, *callRR, *callRU, *callRD, *callZM, *callZP;
callback *callAdd, *callRem, *callEdit, *callTmp, **callPrim[5], *callOK;

unsigned char up[] = { CHR_ARROW_UP,    0 };
unsigned char dn[] = { CHR_ARROW_DOWN,  0 };
unsigned char lf[] = { CHR_ARROW_LEFT,  0 };
unsigned char rg[] = { CHR_ARROW_RIGHT, 0 };

void InitGUI(void)
{
	// Set GUI Style
	paperColor = BLACK; 
	inkColor = WHITE;

	// Title and Frame
  #if defined(__NES__)	
	txtX = 0; txtY = 0; 
	PrintStr("--8bit-Blender--");
	Line((TXT_COLS-10)*FONT_WIDTH, (TXT_COLS-10)*FONT_WIDTH, 5*FONT_HEIGHT, (TXT_ROWS)*FONT_HEIGHT);   
  #else
	txtX = 8; txtY = 0; 
	PrintStr("8bit-Blender");
	Line(0*FONT_WIDTH, 8*FONT_WIDTH, 3, 3); 
	Line(20*FONT_WIDTH, (TXT_COLS-17)*FONT_WIDTH, 3, 3);
	Line((TXT_COLS-10)*FONT_WIDTH+1, (TXT_COLS-10)*FONT_WIDTH+1, 5*FONT_HEIGHT, (TXT_ROWS)*FONT_HEIGHT); 
  #endif
		
	// Show Transform Buttons
	txtX = TXT_COLS-15, txtY = 0; PrintChr('-');
	callRL = Button(TXT_COLS-16, 0, 1, 1, lf);
	callRR = Button(TXT_COLS-14, 0, 1, 1, rg);
	callRU = Button(TXT_COLS-10, 2, 1, 1, up);
	callRD = Button(TXT_COLS-10, 3, 1, 1, dn);
	callZM = Button(TXT_COLS-11, 0, 1, 1, "-");
	callZP = Button(TXT_COLS-10, 0, 1, 1, "+");
	
	// Show Command Buttons
	callAdd  = Button(TXT_COLS-8, 10, 3, 1, "ADD");
	callRem  = Button(TXT_COLS-4, 10, 3, 1, "REM");
	callEdit = Button(TXT_COLS-6, 12, 4, 1, "EDIT");
		
	// Show Memory Stats
	txtX = TXT_COLS-9;
	txtY = TXT_ROWS-2; PrintStr("V:   /256");
	txtY = TXT_ROWS-1; PrintStr("F:   /256");
}

void UpdateGUI(void)
{
	// Show Mesh List and Highlight Current Mesh
	ListBox(TXT_COLS-8, 0, 7, 9, "Meshes", names, nMesh);
	if (nMesh) callTmp = CheckCallbacks(33, iMesh+1);
	
	// Update Memory Stats
	if (nVert > 99) {
		txtX = TXT_COLS-7;
	} else 
	if (nVert > 9) {
		txtX = TXT_COLS-6;
	} else {
		txtX = TXT_COLS-5;
	}	
	txtY = TXT_ROWS-2; PrintNum(nVert);

	if (nFace > 99) {
		txtX = TXT_COLS-7;
	} else 
	if (nFace > 9) {
		txtX = TXT_COLS-6;
	} else {
		txtX = TXT_COLS-5;
	}	
	txtY = TXT_ROWS-1; PrintNum(nFace);	
}

void PrimitivePanel(void)
{
	unsigned char i;

	// Panel
	paperColor = BLACK; inkColor = WHITE;
	Panel(6, 5, 20, 5, " ");
	
	// Buttons
 	for (i=0; i<3; i++) {
		callPrim[i+0] = Button(8, 6+i, 8, 1, namePrim[i+0]);
		if (i<2) callPrim[i+3] = Button(17, 6+i, 8, 1, namePrim[i+3]);
	}
}

unsigned char trsfStr[9][4] = { "0", "0", "0", "0", "0", "0", "10", "10", "10" };

void TransformPanel(void)
{
	unsigned char i,j;
	
	// Panel
	paperColor = BLACK; inkColor = WHITE;
	Panel(6, 5, 20, 5, " ");	
	
	// Labels
	txtX = 7; txtY = 6;
	PrintStr("Pos:"); txtY += 1;
	PrintStr("Rot:"); txtY += 1;
	PrintStr("Dim:"); txtY += 1;
		
	// Inputs
	paperColor = WHITE; inkColor = BLACK;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			callTmp = Input(12+5*j, 6+i, 3, 1, trsfStr[i*3+j], 3);
		}
	}
	
	// Controls
	paperColor = BLACK; inkColor = WHITE;	
	callOK = Button(15, 9, 7, 1, " OK");	
}

void EncodeTransform(void)
{
	unsigned char i;
	fix8 val;	
	for (i=0; i<9; i++) {
		val = trsf[iMesh][i];
		if (i<3 || i>5) val /= 256u;	
		sprintf(trsfStr[i], "%d", val); 
	}
}

unsigned char primitive = 255;

void DecodeTransform(void)
{
	unsigned char i, m;
	fix8 val;
	if (primitive != 255)
		m = nMesh;
	else
		m = iMesh;
	for (i=0; i<9; i++) {
		sscanf(trsfStr[i], "%d", &val); 
		if (i<3 || i>5) val *= 256u;	
		trsf[m][i] = val;
	}
}

extern callback* callHead;

void ResetCalls(void)
{
	unsigned char delete = 0;
	
	// Clear callbacks added after "Edit"
	callback *this = callHead, *next;	
	while (this) {
		next = this->next;
		if (delete)
			free(this);
		if (this == callEdit) {
			this->next = 0;
			delete = 1;
		}
		this = next;
	}	
}

void ProcessCallback(callback* call)
{
	unsigned char i, refresh = 0;
	
	// Check Panel Buttons
	if (call == callOK) { 
		ResetCalls();
		DecodeTransform();
		if (primitive != 255) {
			Push(primitive);
			primitive = 255;
		}
		Transform(iMesh);
		Rasterize(iMesh);		
		RenderAll();
		UpdateGUI();
	} else {
		for (i=0; i<5; i++) {
			if (call == callPrim[i]) {
				ResetCalls();
				primitive = i;
				TransformPanel();
				return;		
			}
		}
	}
	
	// Check Interface Buttons
	if (call == callAdd) {
		PrimitivePanel();
	} else
	if (call == callEdit) {
		if (!nMesh) return;
		EncodeTransform();
		TransformPanel();
	} else 
	if (call == callRem) {
		if (!nMesh) return;
		Pop(); 		
		RenderAll();		
		UpdateGUI();
	} else
	if (call == callRL) {
		camRotZ += 20; refresh = 1;
		camRotZ %= 360;
	} else 
	if (call == callRR) {
		camRotZ -= 20; refresh = 1;
		camRotZ %= 360;
	} else
	if (call == callRU) {
		camRotX += 20; refresh = 1;
		camRotX %= 360;
	} else 
	if (call == callRD) {
		camRotX -= 20; refresh = 1;
		camRotX %= 360;
	} else
	if (call == callZM) {
		camZoom = camZoom*3/2u; refresh = 1;
	} else 
	if (call == callZP) {
		camZoom = camZoom*2/3u; refresh = 1;
	} 

	// Update scene?
	if (refresh) {
		UpdateCamera();	
		RasterizeAll();
		RenderAll();
	}
}

int main(void) 
{
	callback *call;
	unsigned char mouseLock = 0;	

	// Init Screen
	InitBitmap();
	ClearBitmap();
	ShowBitmap();
		
	// Prepare camera
	UpdateCamera();

	// Init systems
	InitMouse();	
	InitGUI();
	UpdateGUI();	
	
	while (1) {		
		// Update Input Box (if active)
		if (ProcessInput()) {
			DisableSprite(0);
		} else {
			// Otherwise update Mouse
			EnableSprite(0);

			// Check callbacks
			if (!(mouse[2] & MOU_LEFT)) {
				if (!mouseLock) {
				#if defined(__APPLE2__) || defined(__ORIC__)
					DisableSprite(0);
				#endif
					call = CheckCallbacks((mouse[0]*TXT_COLS)/160u, (mouse[1]*TXT_ROWS)/200u);
					if (call) {
						if (call->type == CALLTYPE_LISTBOX) {
							iMesh = call->value;
						} else
						if (call->type != CALLTYPE_INPUT) {
							ProcessCallback(call);					
						}
					}
				#if defined(__APPLE2__) || defined(__ORIC__)
					EnableSprite(0);
				#endif			
					mouseLock = 1;
				}
			} else {
				mouseLock = 0;
			}
			
			// Update mouse position
			UpdateMouse();				
		}

	  #if defined(__LYNX__) || defined(__NES__)
		UpdateDisplay(); // Manually refresh Display
	  #endif			
	}
}
