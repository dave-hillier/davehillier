// Font.cpp: implementation of the Font class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <GL/gl.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Font.h"
#include "GLMain.h"
#include "Texture.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Font::Font(char *name, int c, int r, int s)
{
	cols = c;
	rows = r;
	size = s;

	int i,j,k = 0;

	// dimensions
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
		{
			tex_coords[k][0] = (float)j/cols;
			tex_coords[k][1] = (float)i/rows;
			k++;
		}

	// load the textures (name)
	tex_id = texture.LoadTexture(name);
}

Font::~Font()
{

}
void Font::RenderChar (char c, int x, int y)
{
	glBegin (GL_QUADS);

	glTexCoord2f(tex_coords[ch][0], tex_coords[ch][1]+1.0f/rows);
	glVertex2i(x, y);

	glTexCoord2f(tex_coords[ch][0], tex_coords[ch][1]);
	glVertex2i(x, y+size);

	glTexCoord2f(tex_coords[ch][0]+1.0f/cols, tex_coords[ch][1]);
	glVertex2i(x+size, y+size);

	glTexCoord2f(tex_coords[ch][0]+1.0f/cols, tex_coords[ch][1]+1.0f/rows);
	glVertex2i(x+size, y);
	
	glEnd();

}


void Font::Printf(int x, int y, char *format, ...)
{
	char buffer[1000];
	int len;
	va_list		ap;

	va_start(ap, format);
	vsprintf(buffer, format, ap);
	va_end(ap);	

	len = strlen(buffer);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);//GL_ONE_MINUS_SRC_ALPHA);
	// bind the texture
	glEnable(GL_TEXTURE_2D);
	texture.Bind(tex_id);

	int cx = x, cy = y;

	for (int i = 0; i < len; i++)
	{
		if (cx > glapp->width)
		{
			cx = x;
			cy -= size;
		}


		if (buffer[i] == '\n')
			cy -= size;
		else
		{
			cx += size-2;
			RenderChar((int)buffer[i], cx, cy);
		}
	}

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

}
