// Terrain.cpp: implementation of the Terrain class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <glh/glh_extensions.h>
#include <fstream>


#include "glext.h"
#include "types.h"
#include "frustum.h"

#include "new_src/TextureManager.h"
#include "new_src/Targa.h"
#include "new_src/Environment.h"

#include "new_src/Jet.h"
#include "new_src/JetPlayerController.h"
#include "new_src/JetAIController.h"

#include "Camera.h"

#include "Static.h"
#include "Turret.h"
#include "Terrain.h"

int verts = 0;
Terrain terrain;

template<typename T> T Blend(const T& a,const T& b,const T& c) { return a*(1-c) + b*c; }
unsigned char ByteBlend(unsigned char a,unsigned char b,unsigned char c) { return (((a)*(256-(c)) + ((b)*(c))) >> 8); }
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

		

void Terrain::CalcLOD (Vector3f pos)
{
	int x,z;

	for (z = 0; z < z_patches; z++)
	{
		for (x = 0; x < x_patches; x++)
		{
			Patch *p = &patch[(z)*x_patches+x];

			// TODO function this up
			float dist = (1.0f + max (
					abs(p->x+PATCH_SIZE/2 - (int)pos.x), 
					abs(p->z+PATCH_SIZE/2 - (int)pos.z))) / width;

			if (dist > 1.0)
				dist = 1.0;

			float cull = 2.5;//0.750;

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
					Patch *b1, *b2;
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
					Patch *b1, *b2;
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
					Patch *b1, *b2;
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
					Patch *b1, *b2;
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
				Patch *b1, *b2;
				b1 = &patch[(z)*x_patches+x];	
				b2 = &patch[(z+1)*x_patches+x];	
				// check z, z+1
				if (b1->lod - b2->lod >= 1)
					b2->omit_code |= BOTTOM;//LEFT;
			}
			if (z != 0)
			{
				Patch *b1, *b2;
				b1 = &patch[(z)*x_patches+x];	
				b2 = &patch[(z-1)*x_patches+x];
				// check z, z-1
				if (b1->lod - b2->lod >= 1)
					b2->omit_code |= TOP;//RIGHT;
				
			}				
			if (x != (x_patches-1))
			{
				Patch *b1, *b2;
				b1 = &patch[(z)*x_patches+x];	
				b2 = &patch[(z)*x_patches+x+1];
				// check x, x+1
				if (b1->lod - b2->lod >= 1)
					b2->omit_code |= LEFT;//BOTTOM;
			}
			
			if (x != 0)
			{
				Patch *b1, *b2;
				b1 = &patch[(z)*x_patches+x];	
				b2 = &patch[(z)*x_patches+(x-1)];	
				// check x, x-1
				if (b1->lod - b2->lod >= 1)
					b2->omit_code |= RIGHT;//TOP;				
			}
			
		}
	}
}
void Terrain::Render (Vector3f pos)
{
	int i;
	// for each patch
	// ensure no neighbour is any more than 1 level higher than
	// neighbors
	// TODO quadtree
	CalcLOD(pos);

	glColor3f(1,1,1);
	verts = 0;

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	//glEnable(GL_FOG);
	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	TextureManager::Instance().Apply(main_texture_id);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD_SIGNED_EXT);
	TextureManager::Instance().Apply(detail_texture_id);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);

//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_CULL_FACE);

//	reflection = true;
	if (reflection)
	{
		glCullFace(GL_BACK);
		glPushMatrix();
		glScalef(1,-1,1);

		for (i = 0; i < x_patches*z_patches; i++)
		{
			// change for the view culling
			float maxy, miny;
			maxy = patch[i].bounds.max.y;
			miny = patch[i].bounds.min.y;
			
			patch[i].bounds.max.y = -miny;
			patch[i].bounds.max.y = -maxy;

			
			RenderPatch(&patch[i]);

			// restore
			patch[i].bounds.max.y = maxy;
			patch[i].bounds.min.y = miny;
		}
		glPopMatrix();

	}
	glCullFace(GL_FRONT);
	for (i = 0; i < x_patches*z_patches; i++)
	{
		RenderPatch(&patch[i]);
	}


	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
		glDisable(GL_POLYGON_OFFSET_FILL);
//	glDisable(GL_FOG);
	
	//wireframe = true;

	if (wireframe)
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPushMatrix();
		for (i = 0; i < x_patches*z_patches; i++)
		{
			if (patch[i].lod == 0)
				glColor3f(1,1,1);
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
		glPopMatrix();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
bool Terrain::Init (char *name)
{
	const char *rgb=NULL;
	unsigned char *green=NULL, *blue=NULL, *alpha=NULL;
	int bpp, format, i;
	int x,z;
	int ix, iz;

	std::string folder("./maps/");
	folder += name;
	std::string fileName = folder + "/elev.tga";

	Targa tga;
	std::ifstream fileStream(fileName.c_str(), std::ios::binary|std::ios::in);
	if (fileStream.is_open())
	{
		fileStream >> tga;
	
		rgb = tga.data();
		width = tga.width();
		height = tga.height();
	}

	y_scale = 0.125f;

	strcpy(this->name, name); 
	this->name[strlen(this->name)-4] = 0;

	format = GL_RGBA;
	bpp = (format == GL_RGBA) ? 4 : 3;

	data = new byte[width*height];
	normal = new Vector3f[width*height];
	green = new byte[width*height];
	blue = new byte[width*height];
	alpha = new byte[width*height];

	for (i=0; i < width*height*bpp; i += bpp) 
	{
		data[i/bpp] = rgb[i + 0];
		green[i/bpp] = rgb[i + 1]; 
		blue[i/bpp] = rgb[i + 2]; 
		alpha[i/bpp] = rgb[i + 3]; 
	}
	for (i = 0, z = 0; z < height-1; z++) {
		for ( x = 0; x < width-1; x++) {
			switch (green[x+z*width])
			{
			case 0xFF:
				{
					Jet *jet = new Jet(new JetPlayerController);
					jet->org = Vector3f((float)x, (1+data[x+z*width])*y_scale, (float)z);
					jet->ang.y = (360.f*blue[x+z*width])/255.f;
					jet->name = "Player";
					Environment::instance().add(jet);
					Environment::instance().player(jet);

					Actor *pad = new Actor();
					pad->org = jet->org;
					pad->name = "Start";
					Environment::instance().add(pad);
					break;
				}

			case 0x80:
				{
					Jet *jet = new Jet (new JetAIController);
					jet->org = Vector3f((float)x, (1+data[x+z*width])*y_scale, (float)z);
					jet->ang.y = (360.f*blue[x+z*width])/255.f;
					jet->name = "Badguy";
					Environment::instance().add(jet);

					break;
				}

			case 0x81:
				{
					Turret *turret = new Turret (0);
					turret->org = Vector3f((float)x, (data[x+z*width])*y_scale, (float)z);
					turret->ang.y = (360.f*blue[x+z*width])/255.f;
					Environment::instance().add(turret);

					break;
				}
			case 0x82:
				{
					Turret *turret = new Turret (1);
					turret->org = Vector3f((float)x, (data[x+z*width])*y_scale, (float)z);
					turret->ang.y = (360.f*blue[x+z*width])/255.f;
					Environment::instance().add(turret);

					break;
				}
			case 0x83:
				{
					Turret *turret = new Turret (2);
					turret->org = Vector3f((float)x, (data[x+z*width])*y_scale, (float)z);
					turret->ang.y = (360.f*blue[x+z*width])/255.f;

					Environment::instance().add(turret);

					break;
				}
			case 0x84:
				{
					Static *bl = new Static ();
					bl->org = Vector3f((float)x, (1+data[x+z*width])*y_scale, (float)z);
					bl->ang.y = (360.f*blue[x+z*width])/255.f;
					bl->type = 1;
					Environment::instance().add(bl);

					break;
				}
			case 0x85:
				{
					Static *bl = new Static ();
					bl->org = Vector3f((float)x, (data[x+z*width])*y_scale, (float)z);
					bl->ang.y = (360.f*blue[x+z*width])/255.f;
					bl->org.x += (rand() % 255)/255.0f - 1.f;
					bl->org.z += (rand() % 255)/255.0f - 1.f;
					bl->type = 0;
					Environment::instance().add(bl);

					break;
				}				
			case 0x86:
				{
					Static *bl = new Static (2);
					bl->org = Vector3f((float)x, (data[x+z*width])*y_scale, (float)z);
					bl->ang.y = (360.f*blue[x+z*width])/255.f;
					bl->name = "Building";
					Environment::instance().add(bl);

					break;
				}	
			default:
				break;
			} 

		}
	}
	delete [] green;
	delete [] blue;
	delete [] alpha;

	
	
	CalcNormals();

	x_patches = width/(PATCH_SIZE-1);
	z_patches = height/(PATCH_SIZE-1);

	patch = new Patch[x_patches*z_patches];

	i=0;
	for (z = 0; z < height-1; z += (PATCH_SIZE-1))
	{
		for (x = 0; x < width-1; x += (PATCH_SIZE-1))
		{
			i = x/(PATCH_SIZE-1) + x_patches*z/(PATCH_SIZE-1);

			patch[i].x = x;// yes i got confused
			patch[i].z = z;
			patch[i].lod = 4;
			patch[i].min = 255;
			patch[i].max = 0;
			// set them all 0

			patch[i].omit_code = 0;

			for (iz = z; iz < z+PATCH_SIZE; iz++)
			{
				for (ix = x; ix < x+PATCH_SIZE; ix++)
				{
					if (data[iz*width+ix] > patch[i].max)
						patch[i].max = data[iz*width+ix];

					if (data[iz*width+ix] < patch[i].min)
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
	// TODO set repeat etc
	std::string texture = folder + "/texture.jpg";
	std::string textureDetail = folder + "/detail.jpg";

	TextureManager::Instance().Load(texture, main_texture_id);
	TextureManager::Instance().Load(textureDetail, detail_texture_id);

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
inline void Terrain::SetHeight(Vector3f &v)
{
	v.y = GetHeight(v.x, v.z);
}

void Terrain::GetNormal(float x, float z, Vector3f *n) {
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
	Vector3f ab, ac, n;

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

#define PatchVertex(_x,_z) { \
	glColor4f(1, 1, 1, 1);\
	glNormal3fv(normal[_z*width+_x]);\
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, (float)_x/(height/128), (float)_z/(height/128) );\
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB,	((float)_x/width), 1-((float)_z/height) );\
	glVertex3f((float)_x, y_scale*data[_z*width+_x], (float)_z);\
}

void Terrain::RenderPatch (Patch *p)
{
	int step = 1<<p->lod;
	int x,z,i=0;

	int xl=0, xr=0, zt=0, zb=0;


	// culling - use quadtree
	int clip = cliptest(p->bounds);
	if (!clip)
		return;

	/* if (normals_on)
	{
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1,1,1);
		for (x = 0; x < PATCH_SIZE-1; x ++)
		{
			for (z = 0; z < PATCH_SIZE-1; z ++)
			{
				glBegin(GL_LINES);
				glVertex3f(x+p->x, y_scale*data[(p->z+z)*width+p->x+x], z+p->z);
				Vector3f n;
				GetNormal(x+p->x,z+p->z, &n);
				glVertex3f(p->x+x+n.x, y_scale*data[(p->z+z)*width+p->x+x]+n.y, p->z+z+n.z);
				glEnd();
			}
		}
		glEnable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);

	}*/

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
//	
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
Vector3f pick_pos, pick_dir; // Used for picking



// Performs ray-triangle collision detection for landscape picking
bool Terrain::RayTerrain(Vector3f v0, Vector3f v1, Vector3f v2) {

	SetHeight (v0);
	SetHeight (v1);
	SetHeight (v2);
	Vector3f edge1 = v1 - v0;
	Vector3f edge2 = v2 - v0;

	Vector3f pVec = pick_dir % edge2;
	Vector3f qVec;
	Vector3f tVec = Vector3f(pick_pos);
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
bool Terrain::PickExact(Vector3f *point, Bounds& bounds, int planeCheck, int x, int z) {
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
				if (RayTerrain(Vector3f((float)i,		0,(float)j), 
								Vector3f((float)(i+1),	0,(float)j), 
								Vector3f((float)i,		0,(float)(j+1))) ||
					RayTerrain(Vector3f((float)(i+1),	0,(float)(j+1)), 
								Vector3f((float)(i+1),	0,(float)j), 
								Vector3f((float)i,		0,(float)(j+1)))) 
				{
					Vector3f pt = Vector3f((float)i,		0,(float)j);
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
				if (RayTerrain(Vector3f((float)i,		0,(float)j), 
								Vector3f((float)(i+1),	0,(float)j), 
								Vector3f((float)i,		0,(float)(j+1))) ||
					RayTerrain(Vector3f((float)(i+1),	0,(float)(j+1)), 
								Vector3f((float)(i+1),	0,(float)j), 
								Vector3f((float)i,		0,(float)(j+1)))) 
				{
					Vector3f pt = Vector3f((float)i,		0,(float)j);
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
				if (RayTerrain(Vector3f((float)i,		0,(float)j), 
								Vector3f((float)(i+1),	0,(float)j), 
								Vector3f((float)i,		0,(float)(j+1))) ||
					RayTerrain(Vector3f((float)(i+1),	0,(float)(j+1)), 
								Vector3f((float)(i+1),	0,(float)j), 
								Vector3f((float)i,		0,(float)(j+1)))) 
				{
					Vector3f pt = Vector3f((float)i,		0,(float)j);
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
				if (RayTerrain(Vector3f((float)i,		0,(float)j), 
								Vector3f((float)(i+1),	0,(float)j), 
								Vector3f((float)i,		0,(float)(j+1))) ||
					RayTerrain(Vector3f((float)(i+1),	0,(float)(j+1)), 
								Vector3f((float)(i+1),	0,(float)j), 
								Vector3f((float)i,		0,(float)(j+1)))) 
				{
					Vector3f pt = Vector3f((float)i,		0,(float)j);
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

const float terrainEpsilon = 0.001f;

bool Terrain::PickPoint(Vector3f *res, Vector3f start, Vector3f direction) {
	return false; // TODO fix me broken
	Vector3f temp;
	int i, j;
	int planeCheck;

	pick_pos = start;
	pick_dir = direction;
	if (fabs(pick_dir.x) < terrainEpsilon)
		pick_dir.x = 0.00001f;
	if (fabs(pick_dir.z) < terrainEpsilon)
		pick_dir.z = 0.00001f;

	// Get code of sides of bounding boxes to check
	if (pick_dir.x > terrainEpsilon)
		planeCheck = TERRAIN_CHECK_X;
	else
		planeCheck = 0;
	if (pick_dir.z > terrainEpsilon)
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

			if ((temp.z >= patch[i+j*x_patches].bounds.min.z + terrainEpsilon) && 
				(temp.z <= patch[i+j*x_patches].bounds.max.z - terrainEpsilon)) 
			{

				if (pick_dir.y > terrainEpsilon) {
					if ((pick_pos.y <= patch[i+j*x_patches].bounds.max.y - terrainEpsilon) && 
						(temp.y >= patch[i+j*x_patches].bounds.min.y + terrainEpsilon)) 
					{
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				else {
					if ((temp.y <= patch[i+j*x_patches].bounds.max.y - terrainEpsilon) && 
						(pick_pos.y >= patch[i+j*x_patches].bounds.min.y + terrainEpsilon)) 
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
			if ((temp.z >= patch[i+j*x_patches].bounds.min.z + terrainEpsilon) && 
				(temp.z <= patch[i+j*x_patches].bounds.max.z - terrainEpsilon)) {
				if (pick_dir.y > terrainEpsilon) {
					if ((pick_pos.y <= patch[i+j*x_patches].bounds.max.y - terrainEpsilon) && 
						(temp.y >= patch[i+j*x_patches].bounds.min.y + terrainEpsilon)) {
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				else {
					if ((temp.y <= patch[i+j*x_patches].bounds.max.y - terrainEpsilon) && 
						(pick_pos.y >= patch[i+j*x_patches].bounds.min.y + terrainEpsilon)) {
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
			if ((temp.x >= patch[i+j*x_patches].bounds.min.x + terrainEpsilon) && 
				(temp.x <= patch[i+j*x_patches].bounds.max.x - terrainEpsilon)) {
				if (pick_dir.y > terrainEpsilon) {
					if ((pick_pos.y <= patch[i+j*x_patches].bounds.max.y  - terrainEpsilon) && 
						(temp.y >= patch[i+j*x_patches].bounds.min.y + terrainEpsilon)) {
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				else {
					if ((temp.y <= patch[i+j*x_patches].bounds.max.y - terrainEpsilon) && 
						(pick_pos.y >= patch[i+j*x_patches].bounds.min.y + terrainEpsilon)) {
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
			if ((temp.x >= patch[i+j*x_patches].bounds.min.x + terrainEpsilon) && 
				(temp.x <= patch[i+j*x_patches].bounds.max.x - terrainEpsilon)) {
				if (pick_dir.y > terrainEpsilon) {
					if ((pick_pos.y <= patch[i+j*x_patches].bounds.max.y - terrainEpsilon) && 
						(temp.y >= patch[i+j*x_patches].bounds.min.y + terrainEpsilon)) {
						if (PickExact(res, patch[i+j*x_patches].bounds, planeCheck, i, j))
							return true;
					}
				}
				else {
					if ((temp.y <= patch[i+j*x_patches].bounds.max.y - terrainEpsilon) && 
						(pick_pos.y >= patch[i+j*x_patches].bounds.min.y + terrainEpsilon)) {
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


