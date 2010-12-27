// Static.cpp: implementation of the Static class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

//#include "ms3d_model.h"
#include "texturemanager.h"
#include "modelmanager.h"
#include "particlemanager.h"
#include "terrain.h"

#include "Static.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int forest, forest_texture;
int tree, tree_texture;
int building1, building1_dead, building1_texture;

void LoadStaticMedia()
{
	forest = mman.Load("./models/forest.ms3d");
	forest_texture = texture.LoadTexture("./pics/forest.tga");

	tree = mman.Load("./models/tree.ase");
	tree_texture = texture.LoadTexture("./pics/tree.tga");

	building1 = mman.Load("./models/building_1.ms3d");
	building1_dead = mman.Load("./models/building_1_dead.ms3d");
	building1_texture = texture.LoadTexture("./pics/concrete.jpg");

}

Static::Static(int t)
{
	type = t;

	if (type == 2)
	{
		health = 500;
		bounds.max.x = 1000;
		bounds.max.y = 500;
		bounds.max.z = 1000;
		bounds.max *= unit_scale;

		bounds.min.x = -1000;
		bounds.min.y = 0;
		bounds.min.z = -1000;
		bounds.min *= unit_scale;	
	}

}

Static::~Static()
{


}
void DrawBox ()
{
	glBegin(GL_QUADS);
		// Front Face
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		glNormal3f( 0.0f, 0.0f,-1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		// Top Face
		glNormal3f( 0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		// Bottom Face
		glNormal3f( 0.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		// Right face
		glNormal3f( 1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		// Left Face
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();
}
void Static::Render()
{
//	Unit::Render();
	glPushMatrix();

    glTranslatef (org.x,  org.y,  org.z);
	glRotatef (ang.y,  0, 1, 0);
	// depending on type
	glEnable(GL_TEXTURE_2D);

	switch (type)// TODO should really make enum
	{
	case 2:
		glColor3f(1,1,1);
		glScalef(unit_scale, unit_scale, unit_scale);
		texture.Bind(building1_texture);

		if (health > 0)
			mman.Render(building1);
		else
		{
			for (int i=0; i < 10; i++)
			{
				Vector3f pt;
				pt.Set(rand() % 2000 - 1000.0f, 
						250.0f, 
						rand() % 2000 - 1000.0f);
				pt *= unit_scale;
				pt += org;
				pman.Smoke2(pt);
			}

			mman.Render(building1_dead);
		}

		break;
	case 1:
		glDisable(GL_CULL_FACE);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5f);

		glColor3f(1,1,1);
		glScalef(unit_scale*8, unit_scale*8, unit_scale*8);
		texture.Bind(forest_texture);
		mman.Render(forest);

		glDisable(GL_ALPHA_TEST);
		break;
	default:
	case 0:
		glDisable(GL_CULL_FACE);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5f);
		glColor4f(1,1,1,1);
		glScalef(unit_scale*4, unit_scale*4, unit_scale*4);
		// align to the ground
		texture.Bind(tree_texture);
		mman.Render(tree);

		glDisable(GL_ALPHA_TEST);
		break;
	}

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void Static::Update()
{
	// Do nothing
}
