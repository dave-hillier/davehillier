// Console.cpp: implementation of the Console class.
//  This is just the history, nothing to do with the commands
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "Font.h"
#include "glMain.h"
#include "EventScheduler.h"
#include "Input.h"

#include "Console.h"
#include "ConsoleEdit.h"
#include "ConsoleCmds.h"
#include "ConsoleVars.h"
#include "ConsoleAlias.h"

extern Font *font1;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Console *con;

Console::Console()
{
	chars_per_line = 50;
//	con->chars_per_line = glapp->width/13;

	head = 1;
	tail = 0;

	// texures/fonts need to be done else where

	back_scroll = 0;
	screen_lines = 20;
	
	for (int i=0; i < CON_BUFFER_SIZE; i++)
		buffer[i][0] = 0;

	visible = true;//false;

	cedit = new ConsoleEdit();
	command = new ConsoleCmds();
	cvars = new ConsoleVars();
	events = new EventScheduler();
	alias = new ConsoleAliases();

	Printf("Kodama\n------\n");
	Printf("Console Init Done\n");


}

Console::~Console()
{
	delete command;
	delete cvars;
	delete cedit;
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

			// TODO make only print one line at a time
			// ie insert extra \n when required

			font1->Printf(0, glapp->height/2+(font1->GetSize()*line_count), "%s", buffer[i]);
			line_count++;
		}

		cedit->Render();
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
		else if (*ch == '\b')
		{
			len--;
			buffer[head][len] = 0;
		}
		else if (*ch == '\r')
		{
			buffer[head][0] = 0;
		}
		else if (*ch > 20)
		{
			buffer[head][len] = *ch;
			len++;
			buffer[head][len] = 0;
			
			if (len > chars_per_line)
			{
				len = 0;
				NewLine();
			}
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

