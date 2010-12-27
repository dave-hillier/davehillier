#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <malloc.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846	// double Pi
#endif
#define MAX_ENTITIES 1024

#ifndef __cplusplus
typedef enum {false, true} bool; // Only used in C
#endif

#ifdef GL_INCLUDE
#include <gl/gl.h>
#include <gl/glu.h>
#undef GL_INCLUDE
#endif

/*
Window Modes Stuff
*/
#define MAX_MODES 100
typedef unsigned char byte;
typedef struct device_mode_s {
	int	pixelwidth;
	int pixelheight;
	int bitsperpixel;
	int modenum;
} device_mode_t;

typedef struct video_globals_s{
	// Size
	int		width;
	int		height;

	// Corners I dont know if ill use them
	int		top_x;
	int		top_y;
	int		bottom_x;
	int		bottom_y;
	// Duh?
	bool	fullscreen;

	// Windows shite
	HANDLE	hInstance;
	HWND	hWndMain;
	HDC		maindc;
	HGLRC	baseRC;
	// For screenstuff
	DEVMODE devmode;
	device_mode_t modelist[MAX_MODES];
	int		nummodes;
} window_globals_t;

extern window_globals_t win;

typedef enum  {
	VidMode320x200,
	VidMode400x300,
	VidMode640x480,
	VidMode800x600,
	VidMode1024x768,
	VidMode1280x1024,
	VidMode1600x1200,// Why bother?

	MaxVidModes
} video_modes_t;


#define NAME "Project003"
#define TITLE "Project003"

typedef float vector[3];

typedef struct {

	float fov_x, fov_y;
	vector pos, angles;

}view_thing_t;

extern view_thing_t view;

// Vector Macros like quake 2's
#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define VectorAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define VectorCopy(a,b) {b[0]=a[0];b[1]=a[1];b[2]=a[2];}
// probably need inverse, scale, length and cross maybe and set
#define VectorSet(vec, a,b,c) {vec[0]=a;vec[1]=b;vec[2]=c;}


typedef struct entity_s {
	vector	pos, angles, scale;
	int		model_index;// perhaps do an array??
	void	(*think) (struct entity_s *self);
	// effects/texture/frame
} entity_t;

extern entity_t entity_list[MAX_ENTITIES];
extern int	num_entities;
