// ConsoleEdit.cpp: implementation of the ConsoleEdit class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdlib.h>
#include "Input.h"
#include "SDLTimer.h"
#include "GLMain.h"
#include "font.h"
extern Font *font1;

#include "Console.h"
#include "ConsoleCmds.h"
#include "ConsoleEdit.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ConsoleEdit *cedit;

ConsoleEdit::ConsoleEdit()
{
	int i; 
	repeatkey = 0;

	head = 1;
	tail = 0;

	buffer[0] = 0;

	insert = true;
	cursor = 0;

/*	for (i=0 ; i<256 ; i++)
		keyshift[i] = i;*/
	for (i='a' ; i<='z' ; i++)
		keyshift[i] = i - 'a' + 'A';
/*	
	keyshift['1'] = '!';
	keyshift['2'] = '@';
	keyshift['3'] = '#';
	keyshift['4'] = '$';
	keyshift['5'] = '%';
	keyshift['6'] = '^';
	keyshift['7'] = '&';
	keyshift['8'] = '*';
	keyshift['9'] = '(';
	keyshift['0'] = ')';
	keyshift['-'] = '_';
	keyshift['='] = '+';
	keyshift[','] = '<';
	keyshift['.'] = '>';
	keyshift['/'] = '?';
	keyshift[';'] = ':';
	keyshift['\''] = '"';
	keyshift['['] = '{';
	keyshift[']'] = '}';
	keyshift['`'] = '~';
	keyshift['\\'] = '|';
*/
	keyshift['1'] = '!';
	keyshift['2'] = '"';
	keyshift['3'] = '£';
	keyshift['4'] = '$';
	keyshift['5'] = '%';
	keyshift['6'] = '^';
	keyshift['7'] = '&';
	keyshift['8'] = '*';
	keyshift['9'] = '(';
	keyshift['0'] = ')';
	keyshift['-'] = '_';
	keyshift['='] = '+';
	keyshift[','] = '<';
	keyshift['.'] = '>';
	keyshift['/'] = '?';
	keyshift[';'] = ':';
	keyshift['\''] = '@';
	keyshift['['] = '{';
	keyshift[']'] = '}';
	keyshift['`'] = '¬';
	keyshift['\\'] = '|';
	keyshift['#'] = '~';
}

ConsoleEdit::~ConsoleEdit()
{
	 
}

void ConsoleEdit::Render ()
{
	char cstr[128];
	int i;

	for (i = 0; i < 127; i++)
	{
		cstr[i] = ' ';
		cstr[i+1] = '\0';
	}


	if (insert)
		cstr[cursor] = '_';
	else
		cstr[cursor] = '\13';


	// render line
	
	font1->Printf(0, glapp->height/2-20,"%s", buffer);

	// why isbt it displaying?
	// blinks
	if (cursor_blink < timer.GetTime())
	{
		cursor_visible = !cursor_visible;
		cursor_blink = timer.GetTime() + 0.33f;
	}

	if (cursor_visible)
		font1->Printf(0, glapp->height/2-20, "%s", cstr);


	// draw cursor

	// and check for key repeats 
	if ((repeatkey) && (repeatkeytime < timer.GetTime()))
	{
		KeyDown(repeatkey);
		repeatkeytime = timer.GetTime() +.1f;
	}


}

char *GetPasteData( void );

bool ConsoleEdit::KeyDown (int key)
{
	char temp[256];

	repeatkeytime = timer.GetTime() + 0.5f;

	repeatkey = key;
		// convert keypad
	if (key != KEY_TAB)
		tabbed = false;
	if ( ( toupper( key ) == 'V' ) && 
		( input->State(KEY_RCTRL) || input->State(KEY_LCTRL) ) )
	{
		char *cbd;
		
		if ( ( cbd = GetPasteData() ) != 0 )
		{
			int i;

			strtok( cbd, "\n\r\b" );

			i = strlen( cbd );
			if ( i + cursor >= con->chars_per_line)
				i = con->chars_per_line - cursor;

			if ( i > 0 )
			{
				cbd[i]=0;
				strcat( buffer, cbd );
				cursor += i;
				buffer[cursor] = 0;
			}
			delete cbd;//free( cbd );
		}

		return false;
	}

	switch ( key )
	{
	case KEY_KP_MULTIPLY:
		key = '*';
		break;
	case KEY_KP_DIVIDE:
		key = '/';
		break;
	case KEY_KP_MINUS:
		key = '-';
		break;
	case KEY_KP_PLUS:
		key = '+';
		break;
	case KEY_KP7:
		key = '7';
		break;
	case KEY_KP8:
		key = '8';
		break;
	case KEY_KP9:
		key = '9';
		break;
	case KEY_KP4:
		key = '4';
		break;
	case KEY_KP5:
		key = '5';
		break;
	case KEY_KP6:
		key = '6';
		break;
	case KEY_KP1:
		key = '1';
		break;
	case KEY_KP2:
		key = '2';
		break;
	case KEY_KP3:
		key = '3';
		break;
	case KEY_KP0:
		key = '0';
		break;
	case KEY_KP_PERIOD	:
		key = '.';
		break;
	}
	switch (key)
	{
	// also add
	// alt, win, esc, tab, caps, menu, num lock
	case KEY_F1:
	case KEY_F2:
	case KEY_F3:
	case KEY_F4:
	case KEY_F5:
	case KEY_F6:
	case KEY_F7:
	case KEY_F8:
	case KEY_F9:
	case KEY_F10:
	case KEY_F11:
	case KEY_F12:
	case KEY_RCTRL:
	case KEY_LCTRL:
	case KEY_RALT:
	case KEY_LALT:
	case KEY_LSHIFT:
	case KEY_RSHIFT:
	case KEY_MOUSE1:
	case KEY_MOUSE2:
	case KEY_MOUSE3:
	case KEY_MOUSEWHEELUP:
	case KEY_MOUSEWHEELDOWN:
		return true; // non-typing keys

	case KEY_UP:

		if (++history_back_scroll > CON_BUFFER_SIZE)
			history_back_scroll = CON_BUFFER_SIZE;
		{
			char *str = HistoryGetLine();
			strcpy(buffer, str);
		}
		KeyDown(KEY_END);
		break;
	case KEY_DOWN:
		if (--history_back_scroll < 0)
			history_back_scroll = 0;
		{
			char *str = HistoryGetLine();
			strcpy(buffer, str);
		}
		KeyDown(KEY_END);
		break;
	case KEY_RETURN:
	// check for return
		// add to history
		HistoryAddLine(buffer);
		con->Printf("%s\n", buffer);

		// process comand
		command->Execute(buffer);

		buffer[0] = 0;
		cursor = 0;
		history_back_scroll = 0;
		break;
	case KEY_TAB:
		// show a list of possible commands
		{
			int list = ListCommand(tabbed);

			if (list > 1)
			{
				// TODO beep?!
				tabbed = !tabbed;
			}
			else if (list == 1)
			{
				ListCommand();
				tabbed = false;
			}
		}
		break;	// check for history up/down
	case KEY_LEFT:
		--cursor;
		if (cursor < 0)
			cursor = 0;
		break;
	case KEY_RIGHT:
		if (buffer[cursor])
			++cursor;

		break;
	case KEY_INSERT:
		insert = !insert;
		break;
	case KEY_DELETE:// is this the best way of doing it?
		strcpy(temp, &buffer[cursor+1]);
		strcpy(&buffer[cursor], temp);
		break;
	case KEY_HOME:
		cursor = 0;
		break;
	case KEY_END:
		cursor = strlen(buffer);
		break;
	case KEY_BACKSPACE:
		strcpy(temp, &buffer[cursor]);
		if (--cursor < 0)
			cursor = 0;
		strcpy(&buffer[cursor], temp);
		break;
	default:
	// add character to buffer
		if ((input->State(KEY_LSHIFT)) || (input->State(KEY_RSHIFT))) 
			// or capslock
			key = keyshift[key];

		if (key < 21)
			break;

		if (buffer[cursor] == 0)
		{
			buffer[cursor++] = key;
			buffer[cursor] = 0;
		}
		else if (insert)
		{
			// shift along
			strcpy(temp, &buffer[cursor]);
			buffer[cursor] = key;
			strcpy(&buffer[1+cursor++], temp);
		}
		else // overwrite
		{
			buffer[cursor++] = key;
		}
		
		break;
	}
	return false;
}

void ConsoleEdit::KeyUp (int key)
{
	if (repeatkey == key)
		repeatkey = 0;

}

void ConsoleEdit::HistoryAddLine (char *str)
{
	char *dest = history[head];

	strcpy(dest, str);

	head = ++head % CON_BUFFER_SIZE;
	
	if (head == tail)
	{
		history[head][0] = 0;
		tail = ++tail % CON_BUFFER_SIZE;
	}
}

char *ConsoleEdit::HistoryGetLine ()
{
	return history[(head-history_back_scroll)% CON_BUFFER_SIZE];
}



void ConsoleEdit::CompletePartial (char *ptr)
{
	int i, len, count, last=0;
	char buff[20];


	strcpy(buff, ptr);
	len = strlen(buff);

	if (!len)
		return;

	do
	{
		count = 0;
		for (i = 0; i < command->size; i++)
		{
			if ((command->cmds[i].key != -1) && 
				(strncmp(command->cmds[i].name, buff, len) == 0 ))
			{
				count++;
				last = i;
			}

		}
		if (count > 0)
		{
			strncpy(buff, command->cmds[last].name, ++len);
			buff[len] = 0;

			for (i = 0; i < command->size; i++)
			{

				if ((command->cmds[i].key != -1) && 
					(strncmp(command->cmds[i].name, buff, len) == 0 ))
					count--;
			}
		}

	}
	while (count == 0);

	buff[--len] = 0;

	strcpy(ptr, buff);

}

int ConsoleEdit::ListCommand (bool print)
{
	int	i, len, count = 0, found;
	len = strlen(buffer);
	char *ptr = &buffer[len];
	
	
	for (i = len; i; i--)
	{
		if ((*ptr == ' ') || (*ptr == ';') || (*ptr == '"'))
		{
			ptr++;
			break;
		}

		ptr --;
	}
	
	len = strlen(ptr);
	
	if (!len)
		return 0;

	for (i = 0; i < command->size; i++)
	{

		if ((command->cmds[i].key != -1) &&
			(strncmp (ptr, command->cmds[i].name, len) == 0))
		{
			if (print)
				con->Printf("%s\n", command->cmds[i].name);
			count++;
			found = i;
		}
	}

	if (count == 1)
	{
		
		strcpy(ptr, &command->cmds[found].name[0]);
		strcat(ptr, " ");
	//	cursor = strlen(ptr);
		KeyDown(KEY_END);
	}
	else if (count > 1)
	{
		CompletePartial(ptr);
		KeyDown(KEY_END);
	}

	return count;
}