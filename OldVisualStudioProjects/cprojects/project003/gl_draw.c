#include "project003.h"

#include <gl/gl.h>
#include <gl/glu.h>

void Perspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}
void InitViewThing (void)
{
	view.pos[0] = 0;
	view.pos[1] = 0;
	view.pos[2] = 0;

	view.fov_x = 90;// Probably dont need variable fov
	view.fov_y = 90;

	view.angles[0] = 0;
	view.angles[1] = 0;
	view.angles[2] = 0;
}
void InitGL (void)
{
	float aspect;

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST); 

	glShadeModel (GL_SMOOTH);   

    glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
	glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

//	Only do these when you need to
//	glEnable(GL_LINE_SMOOTH);
//	glEnable(GL_POINT_SMOOTH);
//	glEnable(GL_POLYGON_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glShadeModel(GL_FLAT);
  
	glEnable (GL_ALPHA_TEST);

	glClearColor (0.0, 0.0, 0,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
 
    glMatrixMode( GL_PROJECTION ); 
    aspect = (float) win.width / win.height; 
    Perspective( 45.0, aspect, 1,  4096 ); // Use view_thing
    glMatrixMode( GL_MODELVIEW ); 
	InitViewThing();

    inittext();
}
void polarView(GLdouble radius, GLdouble twist, GLdouble latitude, 
           GLdouble longitude) 
{ 
    glTranslated(0.0, 0.0, -radius); 
    glRotated(-twist, 0.0, 0.0, 1.0); 
    glRotated(-latitude, 1.0, 0.0, 0.0); 
    glRotated(longitude, 0.0, 0.0, 1.0);      
 
} 
void pilotView(GLdouble planex, GLdouble planey,
                 GLdouble planez, GLdouble roll,
                 GLdouble pitch, GLdouble heading)
{
      glRotated(roll, 0.0, 0.0, 1.0);
      glRotated(pitch, 0.0, 1.0, 0.0);
      glRotated(heading, 1.0, 0.0, 0.0);
      glTranslated(-planex, -planey, -planez);
}

void drawsticks ()
{
		// Need to do more matricies to set these to proper origins
	glBegin(GL_LINES);
	{
		// This could be the default object? maybe
		glColor3f  (1.0, 0.0, 0.0);
		glVertex3f (0.0, 0.0, 0.0);
		glVertex3f (1.0, 0.0, 0.0);
		glColor3f  (0.0, 1.0, 0.0);
		glVertex3f (0.0, 1.0, 0.0);
		glVertex3f (0.0, 0.0, 0.0);
		glColor3f  (0.0, 0.0, 1.0);
		glVertex3f (0.0, 0.0, 0.0);
		glVertex3f (0.0, 0.0, 1.0);		
		
	}
	glEnd();
} 


void draw_entity (entity_t *e)
{
	glPushMatrix(); 

	glTranslatef (e->pos[0],  e->pos[1],  e->pos[2]);

	glRotatef (e->angles[1],  0, 0, 1);
	glRotatef (-e->angles[0],  0, 1, 0);
	glRotatef (e->angles[2],  1, 0, 0);

	glScalef (e->scale[0], e->scale[1], e->scale[2]);

	// draw_model(e->model_index);
	drawsticks ();

	glPopMatrix(); 
}

void run_entites()
{
	entity_t *e;
	int i;
	for (i = 0, e = &entity_list[0];i < num_entities; i++, e++)
	{
		// Move entitys
		// Do shite
		draw_entity(e);
	}
}

entity_t *get_entity ()
{
	if (num_entities > MAX_ENTITIES)
		return NULL;

	num_entities ++;

	VectorSet (entity_list[num_entities-1].scale, 1, 1, 1);
	VectorSet (entity_list[num_entities-1].pos, 0, 0, 0);
	VectorSet (entity_list[num_entities-1].angles, 0, 0, 0);

	entity_list[num_entities-1].model_index = 0;
	entity_list[num_entities-1].think = NULL;

	return &entity_list[num_entities-1];
}
void RenderScene (void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glPushMatrix(); 

    glRotatef (-view.angles[2],  1, 0, 0);
    glRotatef (-view.angles[0],  0, 1, 0);
    glRotatef (-view.angles[1],  0, 0, 1);
    glTranslatef (-view.pos[0],  -view.pos[1],  -view.pos[2]);
	
	drawsticks();// show origin
	run_entites();

	// End test

	glPopMatrix();

	SwapBuffers(win.maindc);
}