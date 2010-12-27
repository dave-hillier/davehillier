#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
typedef enum { false,true } bool;

typedef struct {
	bool fullscreen;
	int width, height;
	int bpp;
	char desc[64];
} vidmode_t;

void VidInit ();
void VidShutdown ();

#define MAX_VIDMODES	100// temp
#define NAME			"xmaseve"// temp
#define WIDTH			100//640// temp
#define HEIGHT			100//480// temp

extern HDC		ghdc;
extern HGLRC	baseRC;
extern HWND	mainwindow;
extern HINSTANCE ghInstance;
extern vidmode_t modelist[MAX_VIDMODES];
extern int currentmode;

#ifndef M_PI
#define M_PI 3.14159265358979323846 //double pi hmm
#endif
