#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <stdio.h>
#include <stdlib.h>
#include "p6_common.h"

#define MAX(a,b) ((a < b) ? (b) : (a))
#define SQR(a) (a * a)

#define POOL_SIZE	10000 // what size is needed?
#define MAP_SIZE	128 // make variable

extern vec3_t	viewpos;
extern vec3_t  viewangles;

// binary tree with neighbor links
typedef struct TriTreeNode TriTreeNode;
int nexttri, numtris;// for allocation

struct TriTreeNode
{
	TriTreeNode *LeftChild;
	TriTreeNode *RightChild;
	TriTreeNode *BaseNeighbor;
	TriTreeNode *LeftNeighbor;
	TriTreeNode *RightNeighbor;
	int			SplitPriority;
};

TriTreeNode tripool[POOL_SIZE], *base;// perhaps its best to malloc this when map is loaded

byte *heightmap;
byte left_priority[1024], right_priority[1024];// what size?

/*------------------------------------------------------------------------*/
typedef struct {
	TriTreeNode *ptr;
	int			priority;
} TriTreeQueueElement;

typedef struct {
	TriTreeQueueElement Elements[POOL_SIZE];
	int size;
} TriTreeQueue;

TriTreeQueue TreeQueue;

void TTQueueInsert (TriTreeNode *tri, int pri)
{
	int i;
	// check size
	i = TreeQueue.size++;


	while ((i > 1) && 
		(TreeQueue.Elements[i / 2].priority < pri))
	{
		TreeQueue.Elements[i].ptr = TreeQueue.Elements[i / 2].ptr; 
		i /= 2;
	}

	TreeQueue.Elements[i].ptr = tri;
	TreeQueue.Elements[i].priority = pri;
}

TriTreeNode *TTQueueRemove ()
{
	TriTreeNode *ret = TreeQueue.Elements[1].ptr;
	TriTreeQueueElement *tmp = &TreeQueue.Elements[--TreeQueue.size];

	int i = 1, j;
	// check size

	while (i <= TreeQueue.size / 2)
	{
		j = 2 * i;
		if ((j < TreeQueue.size) && 
			 (TreeQueue.Elements[j].priority < TreeQueue.Elements[j+1].priority))
			 j++;
		if (TreeQueue.Elements[j].priority <= tmp->priority)
			break;

		TreeQueue.Elements[i].priority = TreeQueue.Elements[j].priority;
		i = j;
	}

	TreeQueue.Elements[i].priority = tmp->priority;
	TreeQueue.Elements[i].ptr = tmp->ptr;

	return ret;
}


/*------------------------------------------------------------------------*/

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

/*------------------------------------------------------------------------*/

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
/*------------------------------------------------------------------------*/
byte RecursFindZdiff (TriTreeNode *tri, byte *priority,	
		int leftX,  int leftY,  byte leftZ,
		int rightX, int rightY, byte rightZ,
		int apexX,  int apexY,  byte apexZ,
		int node )
{
	int centerX = (leftX + rightX) >>1;		// center of Hypotenuse
	int centerY = (leftY + rightY) >>1;	

	byte centerZ  = heightmap[(centerY * MAP_SIZE) + centerX];
	byte zdiff;

	zdiff = (byte)abs((int)centerZ - (((int)leftZ + (int)rightZ)>>1));

	if ( (abs(leftX - rightX) >= 3) ||
		 (abs(leftY - rightY) >= 3) )
	{
		zdiff = MAX(
			RecursFindZdiff (tri->LeftChild, priority,
				rightX, rightY, rightZ, 
				apexX, apexY, apexZ, 
				centerX, centerY, centerZ, 
				node<<1 ), 
			RecursFindZdiff (tri->RightChild, priority,
				leftX, leftY, leftZ, 
				apexX,   apexY,  apexZ, 
				centerX, centerY, centerZ,    
				1+(node<<1)));
	}
	

	tri->SplitPriority = priority[node] = 1 + zdiff;

	return zdiff;
}
byte RecursCalculateBounds (byte *priority,	
		int leftX,  int leftY,  byte leftZ,
		int rightX, int rightY, byte rightZ,
		int apexX,  int apexY,  byte apexZ,
		int node )
{
	int centerX = (leftX + rightX) >>1;		// center of Hypotenuse
	int centerY = (leftY + rightY) >>1;	

	byte centerZ  = heightmap[(centerY * MAP_SIZE) + centerX];
	byte zdiff;

	zdiff = (byte)abs((int)centerZ - (((int)leftZ + (int)rightZ)>>1));

	if ( (abs(leftX - rightX) >= 3) ||
		 (abs(leftY - rightY) >= 3) )
	{
		zdiff = MAX(
			RecursCalculateBounds (tri->LeftChild, priority,
				rightX, rightY, rightZ, 
				apexX, apexY, apexZ, 
				centerX, centerY, centerZ, 
				node<<1 ), 
			RecursCalculateBounds (tri->RightChild, priority,
				leftX, leftY, leftZ, 
				apexX,   apexY,  apexZ, 
				centerX, centerY, centerZ,    
				1+(node<<1)));
	}
	

	tri->SplitPriority = priority[node] = 1 + zdiff;

	return zdiff;
}
/*------------------------------------------------------------------------*/

void RenderTree ( TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY )
{
	float leftZ  = heightmap[((leftY*MAP_SIZE)+leftX)];
	float rightZ = heightmap[((rightY*MAP_SIZE)+rightX)];
	float apexZ  = heightmap[((apexY*MAP_SIZE)+apexX)];

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
	//	glTexCoord2f(	(GLfloat) leftX/MAP_SIZE, (GLfloat) leftY/MAP_SIZE);

		glVertex3f(		(GLfloat) leftX,
						(GLfloat) leftZ,
						(GLfloat) leftY );

		// Output the RIGHT VERTEX for the triangle
		glColor3d(rightZ/300+0.1,rightZ/300+0.1,rightZ/300+0.1);

	//	glTexCoord2f(	(GLfloat) rightX/MAP_SIZE, 	(GLfloat) rightY/MAP_SIZE);

		glVertex3f(		(GLfloat) rightX,
						(GLfloat) rightZ,
						(GLfloat) rightY );

		// Output the APEX VERTEX for the triangle
		glColor3d(apexZ/300+0.1,apexZ/300+0.1,apexZ/300+0.1);

	//	glTexCoord2f(	(GLfloat) apexX/MAP_SIZE, (GLfloat) apexY/MAP_SIZE);

		glVertex3f(		(GLfloat) apexX,
						(GLfloat) apexZ,
						(GLfloat) apexY );
	}
}
/*------------------------------------------------------------------------*/

void RoamFrame ()
{
	TriTreeNode *tmp;
	int t = 1;
	nexttri = 0;

	// Add to queue
	TreeQueue.size = 1;
	// whats the value of base triangulation
	base = AllocateTri();
	base->BaseNeighbor = AllocateTri();
	base->BaseNeighbor->BaseNeighbor = base;

	RecursCalculateBounds (base, left_priority, 
		0,			0,			heightmap[MAP_SIZE-1 * 0 + 0],// l
		MAP_SIZE,	0,			heightmap[MAP_SIZE-1 * 0 + MAP_SIZE],// r
		0,			MAP_SIZE,	heightmap[MAP_SIZE-1 * MAP_SIZE + 0],// a
		1);

	RecursCalculateBounds (base->BaseNeighbor, right_priority, 
		MAP_SIZE,	MAP_SIZE,	heightmap[MAP_SIZE-1 * MAP_SIZE + MAP_SIZE],// l
		0,			MAP_SIZE,	heightmap[MAP_SIZE-1 * MAP_SIZE + 0],// r
		MAP_SIZE,	0,			heightmap[MAP_SIZE-1 * 0 + MAP_SIZE],// a
		1);



	TTQueueInsert(base, base->SplitPriority);
	TTQueueInsert(base->BaseNeighbor, base->BaseNeighbor->SplitPriority);
	
	t = MAX(base->SplitPriority, base->BaseNeighbor->SplitPriority);
	while (t < 1<<10)
	{
		tmp = TTQueueRemove();
		t = tmp->SplitPriority;
		Split(tmp);
		// Add new tris to queue
		if (tmp->LeftChild)
		{
			TTQueueInsert (tmp->LeftChild,tmp->LeftChild->SplitPriority);
			TTQueueInsert (tmp->RightChild, tmp->RightChild->SplitPriority);
		}

		if ((tmp->BaseNeighbor) && (tmp->BaseNeighbor->LeftChild))
		{
			TTQueueInsert (tmp->BaseNeighbor->RightChild, 
				tmp->BaseNeighbor->RightChild->SplitPriority);

			TTQueueInsert (tmp->BaseNeighbor->LeftChild, 
				tmp->BaseNeighbor->LeftChild->SplitPriority);
		}
	}
	numtris = 0;
	glPushMatrix();
	glScalef(10.0, 5.0, 10.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);

	RenderTree(base, 	
				MAP_SIZE-1,	0,	
				0,			MAP_SIZE-1,
				MAP_SIZE-1,	MAP_SIZE-1);	

	RenderTree(base->BaseNeighbor, 	
				0,			MAP_SIZE-1,	
				MAP_SIZE-1,	0,
				0,			0);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();

}



/*------------------------------------------------------------------------*/

bool RoamInit (char *filename)
{
	FILE *fin;
	// load height map data
	if (fin = fopen(filename, "r"))
	{
		heightmap = malloc (MAP_SIZE * MAP_SIZE);
		fread(heightmap, sizeof(byte), MAP_SIZE * MAP_SIZE, fin);
		fclose(fin);
	}
	else
		return false;
	// load texture



	// init the split and merge queues
	return true;
}

bool RoamShutdown ()
{
	free(heightmap);
	return true;
}