#include "omega_common.h"
#include "omega_gl.h"

// externs
enum { backup, restore };
extern int		font_texture;
extern int numtris;
// defs
#define CON_SCREEN_LINES	20
#define CON_LINES			256
#define CON_BUFFER_SIZE		CON_LINES*CON_LINES_SIZE // hmm bigger smaller??
#define CON_LINES_SIZE		48
#define CON_TEXT_SIZE		12
#define CON_MARGIN			10.0f

// globals
// public
int		ConScroll;
// private
bool	ConEnabled;
char	*console;

int ConBufLines (char *buf)
{
	char *ptr;
	int x, lines=0;

	for (x = 0, ptr = buf; *ptr; x++, ptr++)
	{
		if ((x > CON_LINES_SIZE)||(*ptr == '\n'))
		{
			lines++;
			x = 0;
		}
	}
	return lines;
}


char *eatlines (int num)
{
	char *ptr;
	int i;
	if (num <= 0)
		return console;

	for (i = num, ptr = console; *ptr; ptr++)
	{
		if (*ptr == '\n')
		{
			i--;
			if (i <= 0)
				return ptr+1;
		}

	}
	return console;
}

void ConBack ()
{
	float x1, y1, x2, y2;
	x1 = current_vidmode.width - CON_MARGIN;
	y1 = current_vidmode.height/4 + CON_SCREEN_LINES * CON_TEXT_SIZE;
	x2 = CON_MARGIN;
	y2 = current_vidmode.height/4 - CON_TEXT_SIZE*2;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, current_vidmode.width, 0, current_vidmode.height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	// Need to do more matricies to set these to proper origins
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	glBegin(GL_QUADS);
	{
		// This could be the default object? maybe
		glColor4f  (0.0f,	0.5f,	0.0f,	0.1f);
		glVertex3f (x1, y1,	0.0f);
		glVertex3f (x2,	y1,	0.0f);
		glVertex3f (x2,	y2,	0.0f);
		glVertex3f (x1,	y2,	0.0f);

	}
	glEnd();
	glDisable(GL_BLEND);
	glBegin(GL_LINES);
	{
		glColor4f  (0.0f,	1.0f,	0.0f,	1.0f);
		glVertex3f (x1, y1,	0.0f);
		glVertex3f (x1,	y2,	0.0f);

		glVertex3f (x1,	y1,	0.0f);
		glVertex3f (x2,	y1,	0.0f);
		
		glVertex3f (x2, y2,	0.0f);
		glVertex3f (x2,	y1,	0.0f);

		glVertex3f (x2,	y2,	0.0f);
		glVertex3f (x1,	y2,	0.0f);


	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
void ConRender()
{
	int x, y, yoffset, lines;
	char *ptr = console;

	if (!ConEnabled)
		return;


	ConBack();
	TextSetMode(backup);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, current_vidmode.width, 0, current_vidmode.height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, font_texture);

	// TODO : replace: eatlines and ConBufLines
	// perhaps use a variable which stores the number of used lines
	// and check the size of the last line too
	lines = ConBufLines(console);
	ptr = eatlines((lines - CON_SCREEN_LINES) - ConScroll);

	// dump the text here
	yoffset = CON_SCREEN_LINES - lines;
	if (yoffset < 1)
		yoffset = 1;
	for (y = yoffset; y < CON_SCREEN_LINES; y++)
	{		
		for (x=1;(*ptr) && (x < CON_LINES_SIZE) && (*ptr != '\n'); ptr++, x++)
		{
			int xpos = 10 + CON_TEXT_SIZE * (x - 1);
			int ypos = current_vidmode.height*3/4 - ((y) * CON_TEXT_SIZE);

			RenderChar(*ptr, xpos, ypos, CON_TEXT_SIZE);
		}
		ptr++;
	}
	// print prompt 

	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	TextSetMode(restore);
}
void ConsoleToggle ()
{
	if (ConEnabled)
		ConEnabled = false;
	else
		ConEnabled = true;
}
void ConInit()
{
	ConEnabled = false;

	ConScroll = 0;

	console = (char *)malloc(CON_BUFFER_SIZE);
	memset(console, 0, CON_BUFFER_SIZE);
}
void ConClear ()
{
	//memset(console, 0, CON_BUFFER_SIZE);
	*console = 0;
}
bool ConInput (int wParam)
{
	if (!ConEnabled)
		return false;
	// Manage typing, cursor
	switch (wParam) 
	{ 
	case 27:// esc
		VidShutdown();
		SaveConfig();
		PostQuitMessage(0);
		exit(0);
		break;
	case VK_F6:
		ConPrintf("Position (%.0f %.0f %.0f)\n", viewer.pos[0],
			viewer.pos[1],
			viewer.pos[2]);
		break;
	case VK_F7:
		ConPrintf("Angles (%.0f %.0f %.0f)\n", viewer.angles[0],
			viewer.angles[1],
			viewer.angles[2]);
		break;
	case VK_F9:
		ConsoleToggle();
		break; 
	case VK_F8:
		ConPrintf("Num tris %d\n", numtris);
		break;
	case VK_F10:
		ConClear();
		break;
// What about edit history
	case VK_UP:
		if (ConScroll < ConBufLines(console))
			ConScroll++;
		break;
	case VK_DOWN:
		if (ConScroll > 0)
			ConScroll--;
		break;
	}
//	ConPrintf("%c = %d", wParam, wParam);
	return true;
}

void ConPrintf(char *format, ...)
{
	va_list		argptr;
	char		buffer[1024];
	
	va_start (argptr, format);
	vsprintf (buffer, format,argptr);
	va_end (argptr);

	// TODO: buffer filling up
	strcat(console, buffer);
}
