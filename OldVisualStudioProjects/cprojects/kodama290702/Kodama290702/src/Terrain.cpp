// Terrain.cpp: implementation of the Terrain class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "glext.h"

#include "Console.h"
#include "Texture.h"
#include "Terrain.h"
#include "ConsoleVars.h"
#include "Frustum.h"

ConsoleVar *cv_cull;
ConsoleVar *cv_y_scale;
ConsoleVar *cv_wireframe;

int blocks = 0;
int verts = 0;
Terrain *terrain = NULL;

extern PFNGLMULTITEXCOORD2FARBPROC	glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC	glActiveTextureARB;

#define Blend(_a, _b, _c) ( (_a)*(1-(_c)) + (_b)*(_c) )
#define ByteBlend(_a, _b, _c) ( (((_a)*(256-(_c)) + ((_b)*(_c))) >> 8) )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Terrain::Terrain()
{	

}

Terrain::~Terrain()
{
	// delete/free all stuff created
	delete data;
}

		

void Terrain::CalcLOD (Vector pos)
{
	int x,z;
	float cull = cv_cull->Value();

	for (z = 0; z < z_patches; z++) 
	{
		for (x = 0; x < x_patches; x++)
		{
			patch_t *p = &patch[(z)*x_patches+x];

			float dd = max (
					(float)fabs(p->x+PATCH_SIZE/2 - pos.x), 
					(float)fabs(p->z+PATCH_SIZE/2 - pos.z));

			dd = max(dd, 0.5f*(float)fabs(1.0*p->bounds.max[1]+p->bounds.min[1]/2 - pos.y));

			float dist = (1.0f + dd) / width;


			if (dist > 1.0)
				dist = 1.0;

				

			if (dist < 1.5*(float)PATCH_SIZE/width)
				p->lod = 0;
			else
				p->lod = (int)(cull * (dist+1) * 3 *(255-p->max+p->min)/255);

			if (p->lod > 3)
				p->lod = 3;

			if (p->lod < 0)
				p->lod = 0;

		}
	}


	bool repeat;
	do {
		repeat = false;
			for (z = 0; z < z_patches; z++)
			{
		for (x = 0; x < x_patches; x++)
		{
				// if neighbor has a lower than lod-1
				// raise neighbor lod = lod -1
				// repeat
				if (z != (z_patches-1))
				{
					patch_t *b1, *b2;
					b1 = &patch[(z)*x_patches+x];	
					b2 = &patch[(z+1)*x_patches+x];	
					// check x, x+1
					b1->omit_code = 0;
					b2->omit_code = 0;
					if (b1->lod - b2->lod > 1)
					{
						repeat = true;
						b1->lod = b2->lod + 1;

					}
				}
				if (z != 0)
				{
					patch_t *b1, *b2;
					b1 = &patch[(z)*x_patches+x];	
					b2 = &patch[(z-1)*x_patches+x];
					// check x, x-1
					if (b1->lod - b2->lod > 1)
					{
						repeat = true;
						b1->lod = b2->lod + 1;						
					}

				}				
				if (x != (x_patches-1))
				{
					patch_t *b1, *b2;
					b1 = &patch[(z)*x_patches+x];	
					b2 = &patch[(z)*x_patches+x+1];
					// check z, z+1
					if (b1->lod - b2->lod > 1)
					{
						repeat = true;
						b1->lod = b2->lod + 1;
					}
				}

				if (x != 0)
				{
					patch_t *b1, *b2;
					b1 = &patch[(z)*x_patches+x];	
					b2 = &patch[(z)*x_patches+x-1];	
					// check z, z-1
					if (b1->lod - b2->lod > 1)
					{
						repeat = true;
						b1->lod = b2->lod + 1;
					}

				}

			}
		}
	} while (repeat);

	// disable certain verts
		for (x = 0; x < x_patches; x++)
		{
	for (z = 0; z < z_patches; z++)
	{
			if (z != (z_patches-1))
			{
				patch_t *b1, *b2;
				b1 = &patch[(z)*x_patches+x];	
				b2 = &patch[(z+1)*x_patches+x];	
				// check z, z+1
				if (b1->lod - b2->lod >= 1)
					b2->omit_code |= BOTTOM;//LEFT;
			}
			if (z != 0)
			{
				patch_t *b1, *b2;
				b1 = &patch[(z)*x_patches+x];	
				b2 = &patch[(z-1)*x_patches+x];
				// check z, z-1
				if (b1->lod - b2->lod >= 1)
					b2->omit_code |= TOP;//RIGHT;
				
			}				
			if (x != (x_patches-1))
			{
				patch_t *b1, *b2;
				b1 = &patch[(z)*x_patches+x];	
				b2 = &patch[(z)*x_patches+x+1];
				// check x, x+1
				if (b1->lod - b2->lod >= 1)
					b2->omit_code |= LEFT;//BOTTOM;
			}
			
			if (x != 0)
			{
				patch_t *b1, *b2;
				b1 = &patch[(z)*x_patches+x];	
				b2 = &patch[(z)*x_patches+(x-1)];	
				// check x, x-1
				if (b1->lod - b2->lod >= 1)
					b2->omit_code |= RIGHT;//TOP;				
			}
			
		}
	}
}
void Terrain::Render (Vector pos)
{
	int i;
	// for each patch
	// ensure no neighbour is any more than 1 level higher than
	// neighbors

	// All the fog does is remove the sharp cut off from the end of the view
	// it would still disappear at that distance 
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	float colour[] = {0.0f, 0.5f, 1.0f, 0.0f}; // same as back colour
	glFogfv(GL_FOG_COLOR, colour);
	glFogf(GL_FOG_START, 240.0f);
	glFogf(GL_FOG_END, 300.0f);

	CalcLOD(pos);

//	con->Printf("blocks %d\n", blocks);
	blocks = 0;

	verts = 0;
	y_scale = cv_y_scale->Value();


	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glLineWidth(10);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glColor3f(0,0,0);

	for (i = 0; i < x_patches*z_patches; i++)
	{
		RenderPatch(&patch[i]);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
	

	glEnable(GL_TEXTURE_2D);
	texture.Bind(main_texture_id);

	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_PLANE);
	GLfloat sgenparams[] = {0.0, y_scale*10, 0.0, 0.0};
	glTexGenfv(GL_T, GL_EYE_PLANE, sgenparams);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glCullFace(GL_FRONT);

	glColor3f(1,0,1);
	for (i = 0; i < x_patches*z_patches; i++)
	{
		RenderPatch(&patch[i]);
	}

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_FOG);

	if (cv_wireframe->Value() != 0.0f)
	{
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (i = 0; i < x_patches*z_patches; i++)
		{
			if (patch[i].lod == 0)
				glColor3f(1,1,0);
			else if (patch[i].lod == 1)
				glColor3f(0,0,1);
			else if (patch[i].lod == 2)
				glColor3f(0,1,0);
			else if (patch[i].lod == 3)
				glColor3f(0,1,1);
			else 
				glColor3f(1,0,0);

			RenderPatch(&patch[i]);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


}
bool Terrain::Init (char *name)
{
	char file_name[128];

	byte *rgb = NULL, *red, *green, *blue, *alpha;
	int bpp, format, i;
	int x,z;
	int ix, iz;

	sprintf(file_name, "./maps/%s/elev.tga", name);
	texture.LoadTGA(file_name, &rgb, &width, &height);
	
	/*
	TODO 
	Blit the terrain like this but the entities are only actually over the
	everything must me rendered over each

	+-+-+-+
	| | | |
	+-+-+-+
	| |X| |
	+-+-+-+	
	| | | |
	+-+-+-+
	
	*/

	if (!rgb) {
		// print error
		con->Printf ("Error loading %s\n", name);
		return false;
	}	

	y_scale = 0.125f;
//	y_scale = 64.f/width;

	strcpy(this->name, name); 
	this->name[strlen(this->name)-4] = 0;

/*	if (width != height) {
		return false;
	}*/
	format = GL_RGBA;
	bpp = (format == GL_RGBA) ? 4 : 3;

	/*if (bpp != 4)
	{
		// print 
		return false;
	}*/


	red = new byte[width*height];
	green = new byte[width*height];
	blue = new byte[width*height];
	alpha = new byte[width*height];

	for (i=0; i < width*height*bpp; i += bpp) 
	{
		red[i/bpp] = rgb[i + 0];
		green[i/bpp] = rgb[i + 1]; 
		blue[i/bpp] = rgb[i + 2]; 
		alpha[i/bpp] = rgb[i + 3]; 
	}
	free(rgb);


	// TILE the terrain
/*	data = red;*/
	int rw = width;
	int rh = height;

	width = (3*width-2);
	height = (3*height-2);

	data = new byte[width*height];

	for (z = 0; z < height; z++)
	{
		for (x = 0; x < width; x++)
		{
			int rz = (z%rh);
			int rx = (x%rw);

			data[z*width+x] = red[rz*rw+rx];
		}
	}
#if 0
	{
		// Just a test, and it works
		FILE *fp;
		fp = fopen("e.raw", "wb");

		fwrite(data, width, height, fp);
		fclose(fp);
	}
#endif
	
	// do stuff with other channels here


	delete green;
	delete blue;
	delete alpha;
	
	normal = new Vector[width*height];
	CalcNormals();

	x_patches = width/(PATCH_SIZE-1);
	z_patches = height/(PATCH_SIZE-1);

	patch = new patch_t[x_patches*z_patches];

	i=0;
	for (z = 0; z < height-1; z += (PATCH_SIZE-1))
	{
		for (x = 0; x < width-1; x += (PATCH_SIZE-1))
		{
			i = x/(PATCH_SIZE-1) + x_patches*z/(PATCH_SIZE-1);

			patch[i].x = x;// yes i got confused
			patch[i].z = z;
			patch[i].lod = rand()%4;
			patch[i].min = 255;
			patch[i].max = 0;
			// set them all 0

			patch[i].omit_code = 0;

			for (iz = z; iz < z+PATCH_SIZE; iz++)
			{
				for (ix = x; ix < x+PATCH_SIZE; ix++)
				{
					if (data[iz*width+ix] >= patch[i].max)
						patch[i].max = data[iz*width+ix];

					if (data[iz*width+ix] <= patch[i].min)
						patch[i].min = data[iz*width+ix];
				}
			}

			patch[i].bounds.max.x = (float)x+PATCH_SIZE;
			patch[i].bounds.max.y = (float)y_scale*patch[i].max;
			patch[i].bounds.max.z = (float)z+PATCH_SIZE;

			patch[i].bounds.min.x = (float)x;
			patch[i].bounds.min.y = 0;//(float)y_scale*patch[i].min;
			patch[i].bounds.min.z = (float)z;
		//	i++;
			
		}
	}

	// load texture
	sprintf(file_name, "./maps/%s/texture.jpg", name);
	main_texture_id = texture.LoadTexture(file_name);

	// CVARs dont have to be initilised like this but its how you 
	// do it without overwriting if its already set
	cv_cull = cvars->Get("cull_level");

	if (!cv_cull)
		cv_cull = cvars->Add("cull_level", "1");

	cv_y_scale = cvars->Get("y_scale");

	if (!cv_y_scale)
		cv_y_scale = cvars->Add("y_scale", "0.1");

	cv_wireframe = cvars->Get("wireframe");

	if (!cv_wireframe)
		cv_wireframe = cvars->Add("wireframe", "0");

	return true;
}

float Terrain::GetHeight(float x, float z) {
	if (x < 0)
		x = 0;
	else if (x > width-1)
		x = (float)width-1;

	if (z < 0)
		z = 0;
	else if (z > height-1)
		z = (float)height-1;
	

	int ix = (int )x;
	int iz = (int )z;
	float a, b, c, d;
	float fx = x - ((float )ix);
	float fz = z - ((float )iz);
	float h1, h2;



	if (z < 0)
		z = 0;
	else if (z > (float)height-1)
		z = (float)height-1;

	b = y_scale * data[ix+1 + iz*width];
	c = y_scale * data[ix + (iz+1)*width];
	if (fx < (1.0f-fz)) {
		a = y_scale * data[ix + iz*width];
		d = b+c - a;
	}
	else {
		d = y_scale * data[ix+1 + (iz+1)*width];
		a = b+c - d;
	}
	h1 = Blend(a, b, fx);
	h2 = Blend(c, d, fx);
	
	return Blend(h1, h2, fz);
}
inline void Terrain::SetHeight(Vector &v)
{
	v.y = GetHeight(v.x, v.z);
}

void Terrain::GetNormal(float x, float z, Vector *n) {
	int ix = (int )x;
	int iz = (int )z;
	float fx = x - ((float )ix);
	float fz = z - ((float )iz);
	float h1, h2;

	h1 = Blend(normal[ix+(iz*width)].x, normal[ix+1+(iz*width)].x, fx);
	h2 = Blend(normal[ix+width+(iz*width)].x, normal[ix+1+width+(iz*width)].x, fx);
	n->x = Blend(h1, h2, fz);

	h1 = Blend(normal[ix+(iz*width)].y, normal[ix+1+(iz*width)].y, fx);
	h2 = Blend(normal[ix+width+(iz*width)].y, normal[ix+1+width+(iz*width)].y, fx);
	n->y = Blend(h1, h2, fz);

	h1 = Blend(normal[ix+(iz*width)].z, normal[ix+1+(iz*width)].z, fx);
	h2 = Blend(normal[ix+width+(iz*width)].z, normal[ix+1+width+(iz*width)].z, fx);
	n->z = Blend(h1, h2, fz);
}

void CrossProduct(float *a, float *b, float *c) {
	c[0] = a[1]*b[2] - a[2]*b[1];
	c[1] = a[2]*b[0] - a[0]*b[2];
	c[2] = a[0]*b[1] - a[1]*b[0];
}


void Terrain::CalcNormals(void) {
	int k, x,z, ox, oz;
	float a, b, c, div;
	Vector ab, ac, n;

	if (!normal)
		return;

	for (x=0; x<width; x++) {
		for (z=0; z<height; z++) {
			if (x>0)
				ox=-1;
			else 
				ox=1;
			if (z>0) 
				oz=-width;
			else 
				oz=width;

			k = x+z*width;
			a = data[k]*y_scale;
			b = data[k+ox]*y_scale;
			c = data[k+oz]*y_scale;
			
			ab[0] = 1.0f;
			ab[1] = a - b;
			ab[2] = 0.0f;

			ac[0] = 0.0f;
			ac[1] = a - c;
			ac[2] = 1.0f;

			CrossProduct(ac, ab, n);
			div = (float )sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
			normal[x+z*width][0] = n[0] / div;
			normal[x+z*width][1] = n[1] / div;
			normal[x+z*width][2] = n[2] / div;
		}
	}

}

			// multi texture coordinates here

/*#define PatchVertex(_x,_z) { \
	glColor4f(1, 1, 1, 1);\
	glNormal3fv(normal[_z*width+_x]);\
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, (float)_x/(height/128), (float)_z/(height/128) );\
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	(float)_x/width, (float)_z/height );\
	glVertex3f((float)_x, y_scale*data[_z*width+_x], (float)_z);\
}*/
#define PatchVertex(_x,_z) { \
	glNormal3fv(normal[_z*width+_x]);\
	glVertex3f((float)_x, y_scale*data[_z*width+_x], (float)_z);\
}
void Terrain::RenderPatch (patch_t *p)
{
	int step = 1<<p->lod;
	int x,z,i=0;

	int xl=0, xr=0, zt=0, zb=0;

	int clip = frustum->ClipTest(p->bounds); // TODO quadtree?

	if (!clip)
		return;

	blocks++;

	if (p->lod != 3)
	{
		if (p->omit_code & LEFT)
			xl = step;
		if (p->omit_code & RIGHT)
			xr = step;
		if (p->omit_code & BOTTOM)
			zb = step;
		if (p->omit_code & TOP)
			zt = step;
	}
	else
		p->omit_code = 0;


	for (x = xl; x < PATCH_SIZE-1-xr; x += step)// extra by nature of strips
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (z = zb; z < PATCH_SIZE-zt; z += step)
		{

			PatchVertex((x+p->x), (z+p->z));
			PatchVertex((x+step+p->x), (z+p->z));
			i++;
			verts+=2;

		}
		glEnd();
	}
	
	if (!p->omit_code)
		return;

	// stitch patches together	
	if (p->omit_code & TOP)
	{
		z = PATCH_SIZE-1;
		for (x = 0; x < PATCH_SIZE-1; x += step*2)// extra by nature of strips
		{
			glBegin(GL_TRIANGLE_FAN);
			PatchVertex((x+p->x+step), (z+p->z-step));

			if (x < (PATCH_SIZE-1)-step*2 || !(p->omit_code & RIGHT))
			{
				PatchVertex((x+p->x+step*2), (z+p->z-step));
			}
	
			PatchVertex((x+p->x+step*2), (z+p->z));
			PatchVertex((x+p->x), (z+p->z));

			if (x > 0 || !(p->omit_code & LEFT))
			{
				PatchVertex((x+p->x), (z+p->z-step));
			}


			glEnd();
		}
	}
	if (p->omit_code & BOTTOM)
	{
		z = 0;
		for (x = 0; x < PATCH_SIZE-1; x += step*2)// extra by nature of strips
		{
			glBegin(GL_TRIANGLE_FAN);
			PatchVertex((x+p->x+step), (z+p->z+step));

			if (x > 0 || !(p->omit_code & LEFT))
			{
				PatchVertex((x+p->x), (z+p->z+step));
			}	
			PatchVertex((x+p->x), (z+p->z));
			PatchVertex((x+p->x+step*2), (z+p->z));
			
			if (x < (PATCH_SIZE-1)-step*2 || !(p->omit_code & RIGHT))
			{
				PatchVertex((x+p->x+step*2), (z+p->z+step));
			}




			glEnd();
		}
	}
	if (p->omit_code & LEFT)
	{
		x = 0;
		for (z = 0; z < PATCH_SIZE-1; z += step*2)// extra by nature of strips
		{
			glBegin(GL_TRIANGLE_FAN);
			PatchVertex((x+p->x+step), (z+p->z+step));

			if (z < (PATCH_SIZE-1)-step*2 || !(p->omit_code & TOP))
			{
				PatchVertex((x+p->x+step), (z+p->z+step*2));
			}

			PatchVertex((x+p->x), (z+p->z+step*2));
			PatchVertex((x+p->x), (z+p->z));

			if (z > 0 || !(p->omit_code & BOTTOM))
			{
				PatchVertex((x+p->x+step), (z+p->z));
	
			}


			glEnd();
		}
	}
	if (p->omit_code & RIGHT)
	{
		x = PATCH_SIZE-1;
		for (z = 0; z < PATCH_SIZE-1; z += step*2)
		{
			glBegin(GL_TRIANGLE_FAN);
			PatchVertex((x+p->x-step), (z+p->z+step));

			if (z > 0 || !(p->omit_code & BOTTOM))
			{
				PatchVertex((x+p->x-step), (z+p->z));
			}

			PatchVertex((x+p->x), (z+p->z));
			PatchVertex((x+p->x), (z+p->z+step*2));

			if (z < (PATCH_SIZE-1)-step*2 || !(p->omit_code & TOP))
			{
				PatchVertex((x+p->x-step), (z+p->z+step*2));
			}

			glEnd();
		}
	}
}




//==========================================
#define TERRAIN_CHECK_Z		1
#define TERRAIN_CHECK_X		2
Vector pick_pos, pick_dir; // Used for picking



// Performs ray-triangle collision detection for landscape picking
bool Terrain::RayTerrain(Vector v0, Vector v1, Vector v2) {

	SetHeight (v0);
	SetHeight (v1);
	SetHeight (v2);
	Vector edge1 = v1 - v0;
	Vector edge2 = v2 - v0;

	Vector pVec = pick_dir % edge2;
	Vector qVec;
	Vector tVec = Vector(pick_pos);
	float det, invDet, u, v;

	det = edge1.Dot(pVec);
	
	if (det>-0.00001f && det<0.00001f)
		return false;

	invDet = 1.0f / det;
	
	// calculate distance from vert0 to ray origin
	tVec.Sub(v0);
	
	// calculate U parameter and test bounds
	u = tVec.Dot(pVec) * invDet;
	if (u < 0.0 || u > 1.0)
		return false;
	
	// prepare to test V parameter
	qVec = tVec % edge1;
	
	// calculate V parameter and test bounds
	v = pick_dir.Dot(qVec) * invDet;
	if (v < 0.0 || u+v > 1.0)
		return false;
	
	return true;
}



// Calculates the ray path over a patch and calls RayTerrain() to check exact picking.
bool Terrain::PickExact(Vector *point, Bounds& bounds, int planeCheck, int x, int z) {
	float tx, tz, fi, fj;
	int i, j;//, k;
	int mini, maxi, minj, maxj;

	x *= PATCH_SIZE-1;
	z *= PATCH_SIZE-1;
	i = (int )pick_pos.x;
	j = (int )pick_pos.z;
//	k = i+j*width;
	fi = (float )i;
	fj = (float )j;

	mini = (int )bounds.min.x;
	maxi = (int )bounds.max.x;
	minj = (int )bounds.min.z;
	maxj = (int )bounds.max.z;
	do {
		if (planeCheck & TERRAIN_CHECK_X) {
			tx = fi + 1;
			tz = (tx - pick_pos.x) * (pick_dir.z / pick_dir.x) + pick_pos.z;
//			if ((tz >= fj) && (tz <= fj+1)) {
			if (((float)tx >= (float)fi) && ((float)tx <= (float)(fi+1))) {
				if (RayTerrain(Vector((float)i,		0,(float)j), 
								Vector((float)(i+1),	0,(float)j), 
								Vector((float)i,		0,(float)(j+1))) ||
					RayTerrain(Vector((float)(i+1),	0,(float)(j+1)), 
								Vector((float)(i+1),	0,(float)j), 
								Vector((float)i,		0,(float)(j+1)))) 
				{
					Vector pt = Vector((float)i,		0,(float)j);
					point->Set(pt.x+0.5f, 
								GetHeight(pt.x+0.5f, pt.z+0.5f), 
								pt.z+0.5f);
					return true;
				}
				i += 1;
				fi += 1;
			//	k += 1;
				if (i >= maxi)
					break;
			}
		}
		else {
			tx = fi;
			tz = (tx - pick_pos.x) * (pick_dir.z / pick_dir.x) + pick_pos.z;
//			if ((tz >= fj) && (tz <= fj+1)) {
			if (((float)tx >= (float)fi) && ((float)tx <= (float)(fi+1))) {
				if (RayTerrain(Vector((float)i,		0,(float)j), 
								Vector((float)(i+1),	0,(float)j), 
								Vector((float)i,		0,(float)(j+1))) ||
					RayTerrain(Vector((float)(i+1),	0,(float)(j+1)), 
								Vector((float)(i+1),	0,(float)j), 
								Vector((float)i,		0,(float)(j+1)))) 
				{
					Vector pt = Vector((float)i,		0,(float)j);
					point->Set(pt.x+0.5f, 
								GetHeight(pt.x+0.5f, pt.z+0.5f), 
								pt.z+0.5f);
					return true;
				}
				i -= 1;
				fi -= 1;
			//	k -= 1;
				if (i < mini)
					break;
			}
		}
		if (planeCheck & TERRAIN_CHECK_Z) {
			tz = fj + 1;
			tx = (tz - pick_pos.z) * (pick_dir.x / pick_dir.z) + pick_pos.x;
		//	if ((tx >= fi) && (tx <= fi+1)) {
			if (((float)tx >= (float)fi) && ((float)tx <= (float)(fi+1))) {
				if (RayTerrain(Vector((float)i,		0,(float)j), 
								Vector((float)(i+1),	0,(float)j), 
								Vector((float)i,		0,(float)(j+1))) ||
					RayTerrain(Vector((float)(i+1),	0,(float)(j+1)), 
								Vector((float)(i+1),	0,(float)j), 
								Vector((float)i,		0,(float)(j+1)))) 
				{
					Vector pt = Vector((float)i,		0,(float)j);
					point->Set(pt.x+0.5f, 
								GetHeight(pt.x+0.5f, pt.z+0.5f), 
								pt.z+0.5f);
					return true;
				}
				j += 1;
				fj += 1;
			//	k += width;
				if (j >= maxj)
					break;
			}
		}
		else {
			tz = fj;
			tx = (tz - pick_pos.z) * (pick_dir.x / pick_dir.z) + pick_pos.x;
			if (((float)tx >= (float)fi) && ((float)tx <= (float)(fi+1))) {
				if (RayTerrain(Vector((float)i,		0,(float)j), 
								Vector((float)(i+1),	0,(float)j), 
								Vector((float)i,		0,(float)(j+1))) ||
					RayTerrain(Vector((float)(i+1),	0,(float)(j+1)), 
								Vector((float)(i+1),	0,(float)j), 
								Vector((float)i,		0,(float)(j+1)))) 
				{
					Vector pt = Vector((float)i,		0,(float)j);
					point->Set(pt.x+0.5f, 
								GetHeight(pt.x+0.5f, pt.z+0.5f), 
								pt.z+0.5f);
					return true;
				}
				j -= 1;
				fj -= 1;
			//	k -= width;
				if (j < minj)
					break;
			}
		}
	} while (1);

	return false;
}



bool Terrain::PickPoint(Vector *res, Vector start, Vector direction) {
	Vector temp;
	int i, j;
	int planeCheck;

	pick_pos = start;
	pick_dir = direction;
	if (pick_dir.x == 0.0f)
		pick_dir.x = 0.00001f;
	if (pick_dir.z == 0.0f)
		pick_dir.z = 0.00001f;

	// Get code of sides of bounding boxes to check
	if (pick_dir.x > 0)
		planeCheck = TERRAIN_CHECK_X;
	else
		planeCheck = 0;
	if (pick_dir.z > 0)
		planeCheck |= TERRAIN_CHECK_Z;

	i = (int )(pick_pos.x / (PATCH_SIZE-1));
	j = (int )(pick_pos.z / (PATCH_SIZE-1));

	if (i > x_patches)
		i = x_patches;
	if (i < 0)
		i = 0;
	if (j > z_patches)
		j = z_patches;
	if (j < 0)
		j = 0;

	do {
		// The ray is going right
		if (planeCheck & TERRAIN_CHECK_X) {
		
			temp.x = patch[i+j*x_patches].bounds.max.x;
			temp.y = (temp.x - start.x) * (pick_dir.y / pick_dir.x) + start.y;
			temp.z = (temp.x - start.x) * (pick_dir.z / pick_dir.x) + start.z;

			if ((temp.z >= patch[i+j*x_patches].bounds.min.z) && 
				(temp.z <= patch[i+j*x_patches].bounds.max.z)) 
			{

				if (pick_dir.y > 0) {
					if ((pick_pos.y <= patch[i+j*x_patches].bounds.max.y) && 
						(temp.y >= patch[i+j*x_patches].bounds.min.y)) 
					{
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				else {
					if ((temp.y <= patch[i+j*x_patches].bounds.max.y) && 
						(pick_pos.y >= patch[i+j*x_patches].bounds.min.y)) 
					{
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				i += 1;
				pick_pos.Set(temp);
			}
		}
		// The ray is going left
		else {
			temp.x = patch[i+j*x_patches].bounds.min.x;
			temp.y = (temp.x - start.x) * (pick_dir.y / pick_dir.x) + start.y;
			temp.z = (temp.x - start.x) * (pick_dir.z / pick_dir.x) + start.z;
			if ((temp.z >= patch[i+j*x_patches].bounds.min.z) && 
				(temp.z <= patch[i+j*x_patches].bounds.max.z)) {
				if (pick_dir.y > 0) {
					if ((pick_pos.y <= patch[i+j*x_patches].bounds.max.y) && 
						(temp.y >= patch[i+j*x_patches].bounds.min.y)) {
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				else {
					if ((temp.y <= patch[i+j*x_patches].bounds.max.y) && 
						(pick_pos.y >= patch[i+j*x_patches].bounds.min.y)) {
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				i -= 1;
				pick_pos.Set(temp);
			}
		}

		// The ray is going 'up' (this is infact the +Z pick_direction but it's up if you look from above)
		if (planeCheck & TERRAIN_CHECK_Z) {
			temp.z = patch[i+j*x_patches].bounds.max.z;
			temp.x = (temp.z - start.z) * (pick_dir.x / pick_dir.z) + start.x;
			temp.y = (temp.z - start.z) * (pick_dir.y / pick_dir.z) + start.y;
			if ((temp.x >= patch[i+j*x_patches].bounds.min.x) && 
				(temp.x <= patch[i+j*x_patches].bounds.max.x)) {
				if (pick_dir.y > 0) {
					if ((pick_pos.y <= patch[i+j*x_patches].bounds.max.y) && 
						(temp.y >= patch[i+j*x_patches].bounds.min.y)) {
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				else {
					if ((temp.y <= patch[i+j*x_patches].bounds.max.y) && 
						(pick_pos.y >= patch[i+j*x_patches].bounds.min.y)) {
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				j += 1;
				pick_pos.Set(temp);
			}
		}
		// The ray is going down
		else {
			temp.z = patch[i+j*x_patches].bounds.min.z;
			temp.x = (temp.z - start.z) * (pick_dir.x / pick_dir.z) + start.x;
			temp.y = (temp.z - start.z) * (pick_dir.y / pick_dir.z) + start.y;
			if ((temp.x >= patch[i+j*x_patches].bounds.min.x) && 
				(temp.x <= patch[i+j*x_patches].bounds.max.x)) {
				if (pick_dir.y > 0) {
					if ((pick_pos.y <= patch[i+j*x_patches].bounds.max.y) && 
						(temp.y >= patch[i+j*x_patches].bounds.min.y)) {
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				else {
					if ((temp.y <= patch[i+j*x_patches].bounds.max.y) && 
						(pick_pos.y >= patch[i+j*x_patches].bounds.min.y)) {
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				j -= 1;
				pick_pos.Set(temp);
			}
		}
	} while (i>=0 && i<x_patches && j>=0 && j<x_patches);

	return false;
}


void CmdTerrain (int argc, char *argv[])
{
	if (argc == 2)
	{
		if (terrain)
		{
			delete terrain;

		}

		terrain = new Terrain();

		if (!terrain->Init(argv[1]))
		{
			delete terrain;
			terrain = NULL;
		}
	}
}
