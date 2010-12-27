#include "project003.h"

BOOL InitWindow (HANDLE hInstance, int mode, bool fullscreen, int nCmdShow);

// Globals
entity_t entity_list[MAX_ENTITIES];
int	num_entities = 0;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG        msg; 

	if (!InitWindow (hInstance, VidMode640x480, false, nCmdShow))
		return FALSE;

	InitGL();

	while (1) { 
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
        { 
            if (GetMessage(&msg, NULL, 0, 0) ) 
            { 
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            } else { 
                return TRUE; 
            } 
			
        } 
		RenderScene();
	}
	return TRUE; 
}


// Do time stuff here
