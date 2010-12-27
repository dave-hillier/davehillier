typedef enum {false, true} bool;
typedef float vec3_t[3];
typedef unsigned char byte;

typedef struct {
	bool	fullscreen;// Can this mode go fullscreen?
	int		width, height;
	int		bpp;// bits per pixel
	char	desc[64]; // Quick way of listing the display modes
} vidmode;

typedef struct {
	float	pos[3];
	float	angles[3];
	float	top[3];// normal of the frustum plane
	float	bottom[3];// normal of the frustum plane
	float	left[3];// normal of the frustum plane
	float	right[3];// normal of the frustum plane
	float	fov;
} view;

typedef enum {
	PA_EXPLODE,
	NUM_PART_TYPES,
} pType;

typedef struct _particle{

	float	life;// how long before die
	float	nextmove;// next time to push the particle
	float	start;// for gravity
	float	size;// How big is the bugger

	vec3_t	pos;// ?
	vec3_t	vel;// urg

	float	alpha;// not really used, yet
	bool	gravity;// is gravity on or off

	unsigned int	texture;

	struct	_particle *next;

} particle;

typedef struct _psystem {

	pType		type;
	int			rate;// Particles per frame? cant do 
	float		nextspawn;
	int			num;// Number of particles to spawn before death
	vec3_t		pos;// origin of the particle spawner
	particle	*particles;// list of the particles
	struct		_psystem *next, *prev;

} psystem;

typedef struct {

	vec3_t	gravity;
	psystem	*systems;

} pmanager;


typedef struct {
	int		texid;
	int		height, width;
	char	name[64];
	bool	mipmap;
} texture_t;

typedef struct _TargaHeader {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;

typedef struct {

	char	meshname[64];
	int		texid;
	int		numfaces;
	int		numverts;
	int		numtverts;

	int		*faces;
	float	*verts;
	float	*vnorms;
	float	*tverts;

} mdh_submesh;

typedef struct {

	char		name[64];
	int			nummeshes;
	mdh_submesh *meshes;

} mdh;

typedef struct entity entity;

struct entity {
	bool		in_use;

	vec3_t		position;// Where is it?
	vec3_t		velocity;// speed & direction
	vec3_t		angles;// what dir is it facing
	vec3_t		scale;

	mdh			*model;

	int			effects;// particle trails etc

	vec3_t		mins, maxs;
	// think and next think??
	// sounds
};