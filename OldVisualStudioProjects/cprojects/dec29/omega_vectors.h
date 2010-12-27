typedef float vec3_t[3];

// Vector Macros like quake 2's
#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define VectorAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define VectorCopy(a,b) {b[0]=a[0];b[1]=a[1];b[2]=a[2];}
// TODO:  probably need inverse, scale, length and cross maybe and set
#define VectorSet(vec, a,b,c) {vec[0]=a;vec[1]=b;vec[2]=c;}

#define VectorInverse(vec) {vec[0]*=-1;vec[1]*=-1;vec[2]*=-1;}
#define VectorInverse2(a, b) {b[0]=a[0]*-1;b[1]=a[1]*-1;b[2]=a[2]*-1;}
#define VectorScale(a,b,c) {c[0]=a[0]*b;c[1]=a[1]*b;c[2]=a[2]*b;}
