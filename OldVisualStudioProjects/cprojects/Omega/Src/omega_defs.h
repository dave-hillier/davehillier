#define APPNAME "OmegaProject"


// Vector Macros like quake 2's
// TODO:  probably need length and cross
#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define VectorAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define VectorCopy(a,b) {b[0]=a[0];b[1]=a[1];b[2]=a[2];}
#define VectorSet(vec, a,b,c) {vec[0]=a;vec[1]=b;vec[2]=c;}
#define VectorInverse(vec) {vec[0]*=-1;vec[1]*=-1;vec[2]*=-1;}
#define VectorInverse2(a, b) {b[0]=a[0]*-1;b[1]=a[1]*-1;b[2]=a[2]*-1;}
#define VectorScale(a,b,c) {c[0]=a[0]*b;c[1]=a[1]*b;c[2]=a[2]*b;}

// 
#define MAX_VIDMODES	100
#define MAX_WIDTH		2000
#define MAX_HEIGHT		2000

#define MAX_TEXTURES	1024

#define SQR(x) ((x) * (x))
#define MAX(a,b) ((a < b) ? (b) : (a))
#define DEG2RAD(a) (((a) * M_PI) / 180.0f)

#define PI				3.14159265358979323846

// q defs
#define M_PI			3.14159265358979323846
// up / down
#define	PITCH	0
// left / right
#define	YAW		1
// fall over
#define	ROLL	2


