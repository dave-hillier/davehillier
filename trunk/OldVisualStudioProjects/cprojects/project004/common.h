// includes
// defines

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
// up / down
#define	PITCH	0
// left / right
#define	YAW		1
// fall over
#define	ROLL	2

#define NAME "AppName"
#define TITLE "Daves OpenGL App"
// typedefs
typedef enum {false, true} bool;
typedef unsigned char byte;
typedef float vec3f[3];
typedef float vec3_t[3];// for pinching quake functions
typedef struct {
	char			title[4];
	unsigned int	numfaces;
	unsigned int	numverts;
	unsigned int	numtfaces;
	unsigned int	numtverts;
} mdh_header;

typedef struct mdh {
	unsigned int	numfaces;
	unsigned int	numverts;
	unsigned int	numtfaces;
	unsigned int	numtverts;

	unsigned int	*faces;
	float			*verts;
	float			*vnorms;

	float			*tverts;
	unsigned int	*tfaces;
	
	// Not used yet
	int				texid;
} mdh;

typedef struct mdh_list {
	char			filename[256];
	mdh				*model;
	struct mdh_list	*next;
} mdh_list;

// globals
extern float view_angles[3], view_pos[3];
extern int			width, height;
extern HWND			ghWnd;
extern HINSTANCE	ghInstance;
extern HGLRC		ghGLRC;
extern HDC			ghDC;
// prototypes
void inittext(void);
void TestText(void);
void renderModels();
mdh *getmdhptr (char *filename);



