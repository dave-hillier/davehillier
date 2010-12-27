#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "omega_defs.h"
#include "omega_types.h"

// omega_input.c
void KeyDown (int wParam);
extern int mouseX, mouseY;
// omega_text.c
void TextInit ();
void TextPrint (int x, int y, char *format, ...);

// omega_tga.c
int LoadTGA (FILE *fin);

// omega_gl.c
void glOmegaInit (void);
void glOmegaRenderFrame (void);
extern view			viewer;
// omega_texture.c
void TextureInit (void);
extern int		particle_texture;

// omega_main.c
long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void RunFrame (void);
void LoadConfig (void);
void SaveConfig (void);
extern vidmode		current_vidmode;
extern HDC			ghdc;
extern HGLRC		ghrc;
extern HWND			ghWnd;
extern HINSTANCE	ghInstance;

// omega_vid.c
void VidShutdown (void);
bool VidGoWindowed (void);
bool VidGoFullScreen (void);
void VidInit (void);

// omega_timing.c
extern float fps;
void TimeInit (void);
void CountFPS (void);
extern float res;

// omega_particles.c
psystem *pAddSystem (pType type, vec3_t origin, int rate, int num, psystem *sys);
extern pmanager pManager;
void pRenderSystems (void);
void pRunFrame (void);
void pInitManager (void);


// Console
void ConPrintf(char *format, ...);
void ConInit();
void ConRender();
