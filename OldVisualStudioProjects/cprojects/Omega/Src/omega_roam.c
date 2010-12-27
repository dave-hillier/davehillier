#include "omega_common.h"
#include "omega_gl.h"
#include <math.h>

// Do vertex Arrays
extern TargaHeader	targa_header;
extern byte			*targa_rgba;

#define MAP_SIZE			1024 //
#define HEIGHT_MAP_SIZE		1024
#define PATCH_SIZE			128 //
#define PATCHES_PER_SIDE	(MAP_SIZE/PATCH_SIZE) //

#define POOL_SIZE			262144	//hope its enough, seems to be plenty

typedef struct Wedgie Wedgie;

struct Wedgie{
	int diff;
	Wedgie *left;
	Wedgie *right;
};

Wedgie wedgepool[POOL_SIZE];
int NextWedgie;
Wedgie *AllocateWedgie ()
{
	Wedgie *w;

	if (NextWedgie >= POOL_SIZE)
		return NULL;

	w = &wedgepool[NextWedgie++];

	w->left = w->right = NULL;
	return w;
}

typedef struct TriTreeNode TriTreeNode;
int NextTri;
int numtris;

struct TriTreeNode
{
	TriTreeNode *LeftChild;
	TriTreeNode *RightChild;
	TriTreeNode *BaseNeighbor;
	TriTreeNode *LeftNeighbor;
	TriTreeNode *RightNeighbor;
};

typedef struct {
	TriTreeNode *left;
	TriTreeNode *right;
	Wedgie wedgies;
} patch_t;

int patch_texture[PATCHES_PER_SIDE][PATCHES_PER_SIDE];

byte *heightmap;
TriTreeNode tripool[POOL_SIZE];
patch_t patch[PATCHES_PER_SIDE][PATCHES_PER_SIDE];



TriTreeNode *AllocateTri ()
{
	TriTreeNode *tri;

	if (NextTri >= POOL_SIZE)
		return NULL;

	tri = &tripool[NextTri++];

	tri->BaseNeighbor = NULL;
	tri->LeftChild = tri->RightChild = NULL;
	tri->RightNeighbor = tri->LeftNeighbor = NULL;
	return tri;
}

void Split(TriTreeNode *tri)
{
	// We are already split, no need to do it again.
	if (!tri)
		return;

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


void RenderTree ( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY )
{


	float leftZ  = heightmap[((leftY*MAP_SIZE)+leftX)*HEIGHT_MAP_SIZE/MAP_SIZE];
	float rightZ = heightmap[((rightY*MAP_SIZE)+rightX)*HEIGHT_MAP_SIZE/MAP_SIZE];
	float apexZ  = heightmap[((apexY*MAP_SIZE)+apexX)*HEIGHT_MAP_SIZE/MAP_SIZE ];

	if ( tri->LeftChild )					// All non-leaf nodes have both children, so just check for one
	{
		int centerX = (leftX + rightX)>>1;	// Compute X coordinate of center of Hypotenuse
		int centerY = (leftY + rightY)>>1;	// Compute Y coord...

		RenderTree( tri->LeftChild,  apexX,   apexY, leftX, leftY, centerX, centerY );
		RenderTree( tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY );
	}
	else									// A leaf node!  Output a triangle to be rendered.
	{
		numtris++;

		glColor3d(leftZ/300.0+0.1,leftZ/300.0+0.1,leftZ/300+0.1);

		// Output the LEFT VERTEX for the triangle
		glTexCoord2f(	(GLfloat) leftX/PATCH_SIZE, (GLfloat) leftY/PATCH_SIZE);

		glVertex3f(		(GLfloat) leftX,
						(GLfloat) leftZ,
						(GLfloat) leftY );

		// Output the RIGHT VERTEX for the triangle
		glColor3d(rightZ/300+0.1,rightZ/300+0.1,rightZ/300+0.1);

		glTexCoord2f(	(GLfloat) rightX/PATCH_SIZE, 	(GLfloat) rightY/PATCH_SIZE);

		glVertex3f(		(GLfloat) rightX,
						(GLfloat) rightZ,
						(GLfloat) rightY );

		// Output the APEX VERTEX for the triangle
		glColor3d(apexZ/300+0.1,apexZ/300+0.1,apexZ/300+0.1);

		glTexCoord2f(	(GLfloat) apexX/PATCH_SIZE, (GLfloat) apexY/PATCH_SIZE);

		glVertex3f(		(GLfloat) apexX,
						(GLfloat) apexZ,
						(GLfloat) apexY );
	}
}
void ComputeWedgies (Wedgie *w,
					 int leftX, int leftY, int rightX, int rightY, int apexX, int apexY)
{
	int leftZ  = heightmap[((leftY*MAP_SIZE)+leftX)*HEIGHT_MAP_SIZE/MAP_SIZE];
	int rightZ = heightmap[((rightY*MAP_SIZE)+rightX)*HEIGHT_MAP_SIZE/MAP_SIZE];
	int apexZ  = heightmap[((apexY*MAP_SIZE)+apexX)*HEIGHT_MAP_SIZE/MAP_SIZE ];

}
void TestBuildTree (TriTreeNode *tri, 
					int leftX, int leftY, int rightX, int rightY, int apexX, int apexY)
{
	int centerX = (leftX + rightX)>>1;	// Compute X coordinate of center of Hypotenuse
	int centerY = (leftY + rightY)>>1;	// Compute Y coord...

	float distance = sqrt( SQR((float)centerX - viewer.pos[0]) +
						   SQR((float)centerY - viewer.pos[2]) );

	Split (tri);
	/*
	if (tri->LeftChild)
	{
		TestBuildTree(tri->RightChild,//LeftChild, 
			rightX, rightY, apexX, apexY, centerX, centerY);
		TestBuildTree(tri->LeftChild,//RightChild,  
			apexX,  apexY, leftX, leftY, centerX, centerY);
	}*/
}
bool CullPatch (vec3_t a)
{
	vec3_t b, c, d;

	VectorSet(b, a[0]+PATCH_SIZE,	a[1], a[2]+PATCH_SIZE);
	VectorSet(c, a[0],				a[1], a[2]+PATCH_SIZE);
	VectorSet(d, a[0]+PATCH_SIZE,	a[1], a[2]);


	if ( pointinfrustum(b) || pointinfrustum(c) || pointinfrustum(d) )
		return false;
	else
		return true;
}

void RoamFrame()
{
	int x,y;
	int pos = viewer.pos[2]*1024 + viewer.pos[0];
	TriTreeNode *left, *right;
	vec3_t pt1;
	return;

	if (viewer.pos[0] > 950)
		viewer.pos[0] = 950;
	if (viewer.pos[2] > 950)
		viewer.pos[2] = 950;	
	if (viewer.pos[0] < 50)
		viewer.pos[0] = 50;
	if (viewer.pos[2] < 50)
		viewer.pos[2] = 50;

	numtris = 0;
	NextTri = 0;

//	viewer.pos[1] = heightmap[pos] + 75;

	for (x = 0; x < PATCHES_PER_SIDE; x++)
	{
		for (y = 0; y < PATCHES_PER_SIDE; y++)
		{
			TriTreeNode *left, *right;
			left = patch[x][y].left = AllocateTri();
			right = patch[x][y].right = AllocateTri();
			left->BaseNeighbor = right;
			right->BaseNeighbor = left;
		}
	}

	glEnable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
//	glEnable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);

	glPushMatrix();
	for (x = 0; x < PATCHES_PER_SIDE; x++)
	{
		for (y = 0; y < PATCHES_PER_SIDE; y++)
		{
			if ( x > 0 )
				patch[x][y].left->LeftNeighbor = patch[x-1][y].right;//swap
			else
				patch[x][y].left->LeftNeighbor = NULL;		// Link to bordering Landscape here..

			if ( x < (PATCHES_PER_SIDE-1) )
				patch[x][y].right->LeftNeighbor = patch[x+1][y].left;
			else
				patch[x][y].right->LeftNeighbor = NULL;		// Link to bordering Landscape here..

			if ( y > 0 )
				patch[x][y].left->RightNeighbor = patch[x][y-1].right;
			else
				patch[x][y].left->RightNeighbor = NULL;		// Link to bordering Landscape here..

			if ( y < (PATCHES_PER_SIDE-1) )
				patch[x][y].right->RightNeighbor = patch[x][y+1].left;
			else
				patch[x][y].right->RightNeighbor = NULL;	// Link to bordering Landscape here..
/*	*/	}
	}
	for (x = 0; x < PATCHES_PER_SIDE; x++)
	{
		for (y = 0; y < PATCHES_PER_SIDE; y++)
		{
			left = patch[x][y].left;
			right = patch[x][y].right;
			// link patches, build trees, render trees
			pt1[0] = (x)*(PATCH_SIZE-1);
			pt1[1] = heightmap[(((y)*(PATCH_SIZE-1)*MAP_SIZE)+(x)*(PATCH_SIZE-1))*HEIGHT_MAP_SIZE/MAP_SIZE ];
			pt1[2] = (y)*(PATCH_SIZE-1);
			if (!CullPatch(pt1))// patch is visible
			{
				
				TestBuildTree(left, 
					(x)*(PATCH_SIZE-1),			(y+1)*(PATCH_SIZE-1), 
					(x+1)*(PATCH_SIZE-1),		(y)*(PATCH_SIZE-1), 
					(x)*(PATCH_SIZE-1),			(y)*(PATCH_SIZE-1));
				TestBuildTree(right, 
					(x+1)*(PATCH_SIZE-1),		(y)*(PATCH_SIZE-1), 
					(x)*(PATCH_SIZE-1),			(y+1)*(PATCH_SIZE-1), 
					(x+1)*(PATCH_SIZE-1),		(y+1)*(PATCH_SIZE-1));
			
				glBindTexture(GL_TEXTURE_2D, patch_texture[x][y]);

				glBegin(GL_TRIANGLES);
				{
					RenderTree(left, 
						(x)*(PATCH_SIZE-1),			(y+1)*(PATCH_SIZE-1), 
						(x+1)*(PATCH_SIZE-1),		(y)*(PATCH_SIZE-1), 
						(x)*(PATCH_SIZE-1),			(y)*(PATCH_SIZE-1));
					RenderTree(right, 
						(x+1)*(PATCH_SIZE-1),		(y)*(PATCH_SIZE-1), 
						(x)*(PATCH_SIZE-1),			(y+1)*(PATCH_SIZE-1), 
						(x+1)*(PATCH_SIZE-1),		(y+1)*(PATCH_SIZE-1));
				}
				glEnd();
			}

		}
	}
	glPopMatrix();
	glDisable(GL_CULL_FACE);
}
bool WorldCollision (vec3_t point)
{
	int pos = point[2]*MAP_SIZE + point[0];
	if (point[2] < 0 || point[2] > MAP_SIZE-50)
		return true;
	else if (point[0] < 0 || point[0] > MAP_SIZE-50)
		return true;
//	else if (point[1] > heightmap[pos])
//		return false;
	else
		return true;
}
void RoamInit (char *filename)
{	
	int i, j;
	FILE *fp;
	char cutname[65];
return;
	if (fp = fopen(filename, "rb"))
	{
		heightmap = (byte *)malloc(HEIGHT_MAP_SIZE * HEIGHT_MAP_SIZE);
		fread(heightmap, HEIGHT_MAP_SIZE, HEIGHT_MAP_SIZE, fp);
		fclose(fp);
	}
	strcpy(cutname, filename);
	cutname[strlen(filename)-4] = 0;
	for (i = 0; i < MAP_SIZE/PATCH_SIZE; i++)
		for (j = 0; j < MAP_SIZE/PATCH_SIZE; j++)
		{
			char texture_name[65];

			sprintf(texture_name, "%s_%d_%d.tga", cutname, i, j);
			patch_texture[i][j] = LoadTexture(texture_name, true);
		}

	numtris = 0;
	NextTri = 0;
}

// Do unload roam