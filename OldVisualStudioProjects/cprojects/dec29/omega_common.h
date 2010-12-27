#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "omega_defs.h"
#include "omega_types.h"

// omega_input.c
void KeyDown (int wParam);

// omega_text.c
void TextInit ();
void TextPrint (int x, int y, char *format, ...);

// omega_tga.c
void LoadTGA (FILE *fin);
void Load8bitTGA (FILE *fin);

// omega_gl.c

// omega_texture.c
// omega_main.c
long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void RunFrame (void);
void LoadConfig (void);
void SaveConfig (void);
extern vidmode		current_vidmode;

// omega_vid.c
// omega_timing.c
extern float fps;
// omega_particles.c
psystem *pAddSystem (pType type, vec3_t origin, int rate, int num, psystem *sys);
extern pmanager pManager;

