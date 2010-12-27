#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "p6_common.h"

static int	text_pts[256][2];
bool		italic;
bool		bold;
GLfloat		text_colour[4];
GLfloat		gradient_colour[4];
GLfloat		shadow_colour[4];
static int	font_texture;

#define CHAR_SIZE 16

void initText ()
{
	int i, j, k=0;
	// put the texcoordinates in an array for fast look up
	for (i = 0; i < CHAR_SIZE; i++)
		for (j = 0; j < CHAR_SIZE; j++)
		{
			text_pts[k][0] = CHAR_SIZE*j;
			text_pts[k][1] = CHAR_SIZE*i;
			k++;
		}

	italic = 0;

	text_colour[0] = 0.3f;
	text_colour[1] = 0.3f;
	text_colour[2] = 0.3f;
	text_colour[3] = 1.0f;

	gradient_colour[0] = 1.0f;
	gradient_colour[1] = 1.0f;
	gradient_colour[2] = 1.0f;
	gradient_colour[3] = 1.0f;

	shadow_colour[0] = 0.5f;
	shadow_colour[1] = 0.5f;
	shadow_colour[2] = 0.5f;
	shadow_colour[3] = 0.5f;

	font_texture = GetTexture("fonth.tga");
}

void drawChar (int x, int y, int size, int ch)
{

	glBegin (GL_QUADS);
		glColor4fv(text_colour);

		glTexCoord2f((text_pts[ch][0])/256.0f,
			(text_pts[ch][1] + CHAR_SIZE)/256.0f);
		glVertex2i(x, y);

		glTexCoord2f((text_pts[ch][0] + CHAR_SIZE)/256.0f,
			(text_pts[ch][1] + CHAR_SIZE)/256.0f);
		glVertex2i(x + size, y);

		glColor4fv(gradient_colour);

		glTexCoord2f((text_pts[ch][0] + CHAR_SIZE)/256.0f,
			(text_pts[ch][1])/256.0f);

		glVertex2i(x + (italic ? size/4 : 0) + size, y + size);

		glTexCoord2f(text_pts[ch][0]/256.0f, 
			text_pts[ch][1]/256.0f);

		glVertex2i(x + (italic ? size/4 : 0), y + size);
	glEnd ();
}

// improve this for new lines
void drawString (int x, int y, char *str)
{
	char *ch;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, font_texture);

	for (ch = str; *ch; ch++, x += 13)
	{
		drawChar(x, y, 15, (int)*ch); 
		if (bold)
			drawChar(x+1, y, 15, (int)*ch); 
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
//	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}
 