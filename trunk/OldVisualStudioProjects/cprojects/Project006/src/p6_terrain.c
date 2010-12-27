#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "p6_common.h"

// adaptive like roam
#define MAX(a,b) ((a < b) ? (b) : (a))
#define SQR(a) (a * a)
//
extern vec3_t	viewpos;
extern vec3_t  viewangles;

int mapsize;// squaremaps
#define PATCH_SIZE 128

byte *heightmap;

#define DEPTH 9
float FrameLOD = 50;
int desiredtris = 1500;
// Binary tree structure -------------------------------------------------

int nexttri;
#define POOL_SIZE	10000 // what size is needed?
typedef struct TriTreeNode TriTreeNode;

struct TriTreeNode
{
	TriTreeNode *LeftChild;
	TriTreeNode *RightChild;
	TriTreeNode *BaseNeighbor;
	TriTreeNode *LeftNeighbor;
	TriTreeNode *RightNeighbor;
};
TriTreeNode tripool[POOL_SIZE];


TriTreeNode *AllocateTri ()
{
	TriTreeNode *tri;

	if (nexttri >= POOL_SIZE)
		return NULL;

	tri = &tripool[nexttri++];

	tri->BaseNeighbor = NULL;
	tri->LeftChild = tri->RightChild = NULL;
	tri->RightNeighbor = tri->LeftNeighbor = NULL;
	return tri;
}

// Set priorities
void Split(TriTreeNode *tri)
{
	// the edge
	if (!tri)
		return;

	// We are already split, no need to do it again.
	if (tri->LeftChild)
		return;

	// If this triangle is not in a proper diamond, force split our base neighbor
	if ( tri->BaseNeighbor && (tri->BaseNeighbor->BaseNeighbor != tri) )
		Split(tri->BaseNeighbor);

	// Create children and link into mesh
	tri->LeftChild  = AllocateTri();
	tri->RightChild = AllocateTri();

	// If creation failed, just exit.
	if ( !tri->LeftChild )
		return;

	// Fill in the information we can get from the parent (neighbor pointers)
	tri->LeftChild->BaseNeighbor  = tri->LeftNeighbor;
	tri->LeftChild->LeftNeighbor  = tri->RightChild;

	tri->RightChild->BaseNeighbor  = tri->RightNeighbor;
	tri->RightChild->RightNeighbor = tri->LeftChild;

	// Link our Left Neighbor to the new children
	if (tri->LeftNeighbor != NULL)
	{
		if (tri->LeftNeighbor->BaseNeighbor == tri)
			tri->LeftNeighbor->BaseNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->LeftNeighbor == tri)
			tri->LeftNeighbor->LeftNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->RightNeighbor == tri)
			tri->LeftNeighbor->RightNeighbor = tri->LeftChild;
		else
			;// Illegal Left Neighbor!
	}

	// Link our Right Neighbor to the new children
	if (tri->RightNeighbor != NULL)
	{
		if (tri->RightNeighbor->BaseNeighbor == tri)
			tri->RightNeighbor->BaseNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->RightNeighbor == tri)
			tri->RightNeighbor->RightNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->LeftNeighbor == tri)
			tri->RightNeighbor->LeftNeighbor = tri->RightChild;
		else
			;// Illegal Right Neighbor!
	}

	// Link our Base Neighbor to the new children
	if (tri->BaseNeighbor != NULL)
	{
		if ( tri->BaseNeighbor->LeftChild )
		{
			tri->BaseNeighbor->LeftChild->RightNeighbor = tri->RightChild;
			tri->BaseNeighbor->RightChild->LeftNeighbor = tri->LeftChild;
			tri->LeftChild->RightNeighbor = tri->BaseNeighbor->RightChild;
			tri->RightChild->LeftNeighbor = tri->BaseNeighbor->LeftChild;
		}
		else
			Split( tri->BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
	}
	else
	{
		// An edge triangle, trivial case.
		tri->LeftChild->RightNeighbor = NULL;
		tri->RightChild->LeftNeighbor = NULL;
	}
}

byte RecursCalculateBounds (byte *priority,	
		int leftX,  int leftY,  byte leftZ,
		int rightX, int rightY, byte rightZ,
		int apexX,  int apexY,  byte apexZ,
		int node )
{
	int centerX = (leftX + rightX) >>1;		// center of Hypotenuse
	int centerY = (leftY + rightY) >>1;	

	byte centerZ  = heightmap[(centerY * mapsize-1) + centerX];
	byte zdiff;

	zdiff = (byte)abs((int)centerZ - (((int)leftZ + (int)rightZ)>>1));

	if ( (abs(leftX - rightX) >= 8) ||
		 (abs(leftY - rightY) >= 8) )
	{
		zdiff = MAX(
			RecursCalculateBounds (priority,
				rightX, rightY, rightZ, 
				apexX, apexY, apexZ, 
				centerX, centerY, centerZ, 
				node<<1 ), 
			RecursCalculateBounds (priority,
				leftX, leftY, leftZ, 
				apexX,   apexY,  apexZ, 
				centerX, centerY, centerZ,    
				1+(node<<1)));
	}
	

	priority[node] = 1 + zdiff;

	return zdiff;
}

void RecursTessellate( TriTreeNode *tri, byte *priority,	
							 int leftX,  int leftY,
							 int rightX, int rightY,
							 int apexX,  int apexY,
							 int node )
{
	float TriLOD;

	int centerX = (leftX + rightX)>>1; // Compute X coordinate of center of Hypotenuse
	int centerY = (leftY + rightY)>>1; // Compute Y coord...

	if ( node < (1<<DEPTH) )
	{
		// Extremely slow distance metric (sqrt is used).
		// Replace this with a faster one!
		double distance = 1.0f + sqrt( 
			(centerX - viewpos[0])*(centerX - viewpos[0]) + (centerY - viewpos[2])*(centerY - viewpos[2])
			);
		
		// Egads!  A division too?  What's this world coming to!
		// This should also be replaced with a faster operation.
		TriLOD = ((float)priority[node] * mapsize * 2)/distance;	// Take both distance and variance into consideration
	}

	if ( (node >= (1<<DEPTH)) ||	// IF we do not have variance info for this node, then we must have gotten here by splitting, so continue down to the lowest level.
		 (TriLOD > FrameLOD))	// OR if we are not below the variance tree, test for variance.
	{
		Split(tri);														// Split this triangle.
		
		if (tri->LeftChild &&											// If this triangle was split, try to split it's children as well.
			((abs(leftX - rightX) >= 8) || (abs(leftY - rightY) >= 8)))	// Tessellate all the way down to one vertex per height field entry
		{
			RecursTessellate( tri->LeftChild, priority,
				apexX,  apexY, leftX, leftY, centerX, centerY,    
				node<<1  );
			RecursTessellate( tri->RightChild, priority,
				rightX, rightY, apexX, apexY, centerX, centerY, 
				1+(node<<1) );
		}
	}
}

void RenderTree ( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY )
{


	if (tri->LeftChild )			// All non-leaf nodes have both children, so just check for one
	{
		int centerX = (leftX + rightX)>>1;	// Compute X coordinate of center of Hypotenuse
		int centerY = (leftY + rightY)>>1;	// Compute Y coord...

		RenderTree( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
		RenderTree( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
	}
	else									// A leaf node!  Output a triangle to be rendered.
	{
		float leftZ  = heightmap[((leftY*mapsize-1)+leftX)];
		float rightZ = heightmap[((rightY*mapsize-1)+rightX)];
		float apexZ  = heightmap[((apexY*mapsize-1)+apexX)];

		glColor3d(leftZ/300.0+0.1,leftZ/300.0+0.1,leftZ/300+0.1);

		// Output the LEFT VERTEX for the triangle
		// Single Texture or lots?
		glTexCoord2f(	(GLfloat) leftX/mapsize, (GLfloat) leftY/mapsize);
		glVertex3f(		(GLfloat) leftX,
						(GLfloat) leftZ,
						(GLfloat) leftY );

		// Output the RIGHT VERTEX for the triangle
		glColor3d(rightZ/300+0.1,rightZ/300+0.1,rightZ/300+0.1);

		glTexCoord2f(	(GLfloat) rightX/mapsize, 	(GLfloat) rightY/mapsize);
		glVertex3f(		(GLfloat) rightX,
						(GLfloat) rightZ,
						(GLfloat) rightY );

		// Output the APEX VERTEX for the triangle
		glColor3d(apexZ/300+0.1,apexZ/300+0.1,apexZ/300+0.1);

		glTexCoord2f(	(GLfloat) apexX/mapsize, (GLfloat) apexY/mapsize);
		glVertex3f(		(GLfloat) apexX,
						(GLfloat) apexZ,
						(GLfloat) apexY );
	}
}

// Patch structure --------------------------------------------------------
typedef struct Patch Patch;

struct Patch {

	byte zdiff_right[1 << DEPTH];
	byte zdiff_left [1 << DEPTH];

	TriTreeNode *left;
	TriTreeNode *right;

	bool deform;
	bool visible;
};
#define MAX_PATCHES 16
// hey memory is cheap, figure out how to dynamically allocate
// max mapsize = 2048
Patch landscape[MAX_PATCHES][MAX_PATCHES];
// Terrain ----------------------------------------------------------------
int orientation( int pX, int pY, int qX, int qY, int rX, int rY )
{
	int aX, aY, bX, bY;
	float d;

	aX = qX - pX;
	aY = qY - pY;

	bX = rX - pX;
	bY = rY - pY;

	d = (float)aX * (float)bY - (float)aY * (float)bX;
	return (d < 0) ? (-1) : (d > 0);
}

void TerrainFrame ()
{
	int i, j, x, z;
	nexttri = 0;

	for (i = 0, x = 0; i < mapsize; i+=PATCH_SIZE, x++ )
	{
		for (j = 0, z = 0; j < mapsize; j+=PATCH_SIZE, z++)
		{
			Patch *patch = &landscape[z][x];
			patch->left = AllocateTri();
			patch->right = AllocateTri();

			patch->right->BaseNeighbor = patch->left;
			patch->left->BaseNeighbor = patch->right;

			if (i > j)
				patch->visible = true;
			else
				patch->visible = false;
		}
	}
	for (i = 0, x = 0; i < mapsize; i+=PATCH_SIZE, x++ )
	{
		for (j = 0, z = 0; j < mapsize; j+=PATCH_SIZE, z++)
		{
			Patch *patch = &landscape[z][x];

			if (patch->visible)
			{
				// check for recalc deformation
				if ( x > 0 )
					patch->left->LeftNeighbor = landscape[z][x-1].right;
				else
					patch->left->LeftNeighbor = NULL;		// Link to bordering Landscape here..

				if ( x < (mapsize/PATCH_SIZE-1) )
					patch->right->LeftNeighbor  = landscape[z][x-1].left;
				else
					patch->right->LeftNeighbor  = NULL;		// Link to bordering Landscape here..

				if ( z > 0 )
					patch->left->RightNeighbor = landscape[z][x-1].right;
				else
					patch->left->RightNeighbor = NULL;		// Link to bordering Landscape here..

				if ( z < (mapsize/PATCH_SIZE-1) )
					patch->right->RightNeighbor = landscape[z][x-1].left;
				else
					patch->right->RightNeighbor = NULL;	// Link to bordering Landscape here..
			}
			// check for vis
			// link patches
			// do neighbours
		}
	}
	for (i = 0, x = 0; i < mapsize; i+=PATCH_SIZE, x++ )
	{
		for (j = 0, z = 0; j < mapsize; j+=PATCH_SIZE, z++)
		{
			Patch *patch = &landscape[z][x];

			if (patch->visible)
			{
				// tesselate
				RecursTessellate(patch->left, patch->zdiff_left,
					(i+0),			(j+PATCH_SIZE),
					(i+PATCH_SIZE), (j+0),
					(i+0),			(j+0),
					1);

				RecursTessellate(patch->right, patch->zdiff_right,
					(i+PATCH_SIZE),	(j+0), 
					(i+0),			(j+PATCH_SIZE),
					(i+PATCH_SIZE),	(j+PATCH_SIZE),
					1);

				//glPushMatrix();
				//glScalef(10.0, 5.0, 10.0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glBegin(GL_TRIANGLES);

				// render
				RenderTree(patch->left,
					(i+0),			(j+PATCH_SIZE),
					(i+PATCH_SIZE), (j+0),
					(i+0),			(j+0));
				RenderTree(patch->right,
					(i+PATCH_SIZE),	(j+0),
					(i+0),			(j+PATCH_SIZE),
					(i+PATCH_SIZE),	(j+PATCH_SIZE));

				glEnd();
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				//glPopMatrix();
			}
		}
	}

	
	// calculate num tris rendered/desired tris
	FrameLOD += ((float)nexttri - (float)desiredtris)/(float)desiredtris;
	//(fps - 30)/30;
}

void TerrainInit (char *filename)
{
	int i, j, z, x;
	FILE *fp;
	mapsize = 1024;

	// load heightmap
	if (fp = fopen(filename, "rb"))
	{
		heightmap = (byte *)malloc(mapsize * mapsize);
		fread(heightmap, mapsize, mapsize, fp);
		fclose(fp);
	}

	// RecursCalculateBounds for each patch
	for (i = 0, x = 0; i < mapsize; i+=PATCH_SIZE, x++ )
	{
		for (j = 0, z = 0; j < mapsize; j+=PATCH_SIZE, z++)
		{
			Patch *patch = &landscape[z][x];

			RecursCalculateBounds(patch->zdiff_left, 
				(i+0),			(j+PATCH_SIZE), heightmap[j+PATCH_SIZE * (mapsize-1) + i-1], // l x, y, z
				(i+PATCH_SIZE), (j+0),			heightmap[j * (mapsize-1) + i+PATCH_SIZE-1], // r x, y, z
				(i+0),			(j+0),			heightmap[j * (mapsize-1) + i-1], // a x, y, z
				1);

			RecursCalculateBounds(patch->zdiff_right, 
				(i+PATCH_SIZE), (j+0),			heightmap[j * (mapsize-1) + i+PATCH_SIZE], // l x, y, z
				(i+0),			(j+PATCH_SIZE), heightmap[j+PATCH_SIZE * (mapsize-1) + i], // r x, y, z
				(i+PATCH_SIZE), (j+PATCH_SIZE), heightmap[j+PATCH_SIZE * (mapsize-1) + i+PATCH_SIZE], // a x, y, z
				1);
			patch->left = AllocateTri();
			patch->right = AllocateTri();

			patch->right->BaseNeighbor = patch->left;
			patch->left->BaseNeighbor = patch->right;

		}
	}
}
