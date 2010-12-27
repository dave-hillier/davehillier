#include "project003.h"
#include <stdarg.h>

char *va(char *format, ...)
{
	va_list         argptr;
	static char  string[1024];
	
	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	return string;  
}

void Error (char *format, ...)
{	
	va_list         argptr;
	char             string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	MessageBox(NULL, string, "Error", MB_OK);
	exit(1);
}

void MsgBox (int type, char *title, char *format, ...)
{	
	va_list         argptr;
	char             string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	MessageBox(NULL, string, title, type);
}