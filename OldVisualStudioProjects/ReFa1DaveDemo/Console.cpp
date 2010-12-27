// Console.cpp: implementation of the Console class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "Font.h"
#include "glApp.h"
#include "Console.h"
#include "new_src/DebugLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Console *con;

Console::Console()
{
	head = 1;
	tail = 0;

	// texures/fonts need to be done else where

	back_scroll = 0;
	screen_lines = 20;
	
	for (int i=0; i < CON_BUFFER_SIZE; i++)
		buffer[i][0] = 0;

	visible = false;


	Printf("Console Init Done\n");
	Printf("======\n");
}

Console::~Console()
{

}

void Console::Render ()
{

	if (visible)
	{
		if (back_scroll > CON_BUFFER_SIZE-1-screen_lines)
			back_scroll = CON_BUFFER_SIZE-1-screen_lines;

		int i, line_count = 0, sl;

		sl = head+back_scroll; 
		
		for (i = sl; (screen_lines != line_count) && (i != tail); i--)
		{
			if (i < 0) 
				i += CON_BUFFER_SIZE;

			glPushMatrix();
			glTranslatef(10, 150+(float)(line_count)*15, 10);
			glScalef(20, 20, 0);

			arial->printf("%s", buffer[i]);
			glPopMatrix();   

			line_count++;
		}
	}
}

void Console::Printf (char *format, ...)
{
	char buf[1000];
	va_list		ap;

	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);	

	// duplicate string?
	// parse caridge returns/new lines
	fprintf(stderr, "%s", buf);

	AddLine(buf);
	debugLog << "Con> " << buf;
}

void Console::NewLine()
{
	head = ++head % CON_BUFFER_SIZE;
	
	if (head == tail)
	{
		buffer[head][0] = 0;
		tail = ++tail % CON_BUFFER_SIZE;
	}
}
void Console::AddLine (char *str)
{
	char *ch;
	int len = strlen(buffer[head]);

	for (ch = str; *ch != 0; ch++)
	{
		if (*ch == '\n')
		{
			// increment buffer
			NewLine();
		}
		else if (*ch > 20)
		{
			buffer[head][len] = *ch;
			len++;
			buffer[head][len] = 0;

			if (len > CON_LINE_WIDTH)
				NewLine();
		}

	}
}

void Console::Clear ()
{
	head = 0;
	tail = 0;

	for (int i=0; i < CON_BUFFER_SIZE; i++)
		buffer[i][0] = 0;

}

