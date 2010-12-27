
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "Console.h"
#include "ConsoleCmds.h"
#include "ConsoleVars.h"

int MD4Data (char *buf, int len);
ConsoleVars *cvars;
void CmdSet2 (int argc, char *argv[]);

void ConsoleVar::Set(char *nme, char *str, int flgs)
{
	int len = strlen(nme);

	if (name)
		delete name;

	name = new char[len + len%4]; 

	for (int i = 0; i < len; i++)
		name[i] = tolower(nme[i]);
	name[i] = 0;

	key = MD4Data(name, i);

	len = strlen(str);
	string = new char[len + len%4];
	strcpy(string, str);

	value = (float)atof(string);

	flags = flgs;
}

ConsoleVar::~ConsoleVar ()
{
	if (name)
		delete name;
	if (string)
		delete string;
}

ConsoleVars::ConsoleVars ()
{
	used = 0;
	size = MAX_VARS;

	// set all pointers to 0
	for (int i=0; i < MAX_VARS; i++)
		vars[i] = NULL;
}
ConsoleVars::~ConsoleVars()
{
	for (int i=0; i < MAX_VARS; i++)
	{
		if (vars[i] != NULL)
			delete vars[i];
	}
}

ConsoleVar *ConsoleVars::Get(char *name)
{
	int i;
	ConsoleVar *cv = new ConsoleVar();
	cv->Set(name, "a", 0);

	int index = abs(cv->key % size);
	i = index;

	while (true)
	{
		if (vars[i] && vars[i]->key == cv->key)
		{
			delete cv;
			return vars[i];
		}
		else //if (vars[i])
		{
			i = (i+1) % size;

		}

		if (i == index)
			return NULL;
	}
}

ConsoleVar *ConsoleVars::Add(char *name, char *str, int flgs)
{
	int i;
	ConsoleVar *cv = new ConsoleVar();
	cv->Set(name, str, flgs);

	int index = abs(cv->key % size);
	
	if (used >= size-1)
		return NULL;

	used++;

	i = index;

	// probe, find next free!
	while (true)
	{

		if (vars[i] && vars[i]->key == cv->key)
		{
			int len = strlen(vars[i]->string);
			delete vars[i]->string;
			vars[i]->string = new char[len + len%4];
			strcpy(vars[i]->string, str);
			delete cv;
			// update value
			return vars[i];
		}
		else if (vars[i])
		{
			i = (i+1) % size;
		}
		else 
		{
			vars[i] = cv;
			break;
		}
	}

	command->Add(name, CmdSet2);


	return cv;
}

void CmdSet (int argc, char *argv[])
{
	if (argc >= 3)
	{
		// concat args
		char args[128];
		args[0] = 0;

		for (int i = 2; i < argc; i++)
		{
			strcat(args, argv[i]);
			strcat(args, " ");
		}

		ConsoleVar *cv = cvars->Get(argv[1]);

		if (!cv)
		{
			cvars->Add(argv[1], args, 0);
		}
		else
			cv->Set(argv[1], args, 0);
	}
	else if (argc == 2)
	{
		ConsoleVar *cv = cvars->Get(argv[1]);

		if (cv)
			con->Printf("%s = %s\n", cv->name, cv->string);
		else
			con->Printf("%s is not set\n", argv[1]);

	}
	else
	{
		// print general usage
	}
}

void CmdSet2 (int argc, char *argv[])
{
	if (argc >= 2)
	{
		//  concat args
		char args[128];
		args[0] = 0;

		for (int i = 1; i < argc; i++)
		{
			strcat(args, argv[i]);
			strcat(args, " ");
		}
		ConsoleVar *cv = cvars->Get(argv[0]);

		if (!cv)
			cvars->Add(argv[0], args, 0);
		else
			cv->Set(argv[0], args, 0);

	}
	else if (argc == 1)
	{
		ConsoleVar *cv = cvars->Get(argv[0]);
		if (cv)
			con->Printf("%s = %s\n", cv->name, cv->string);
		else
			con->Printf("%s is not set\n", argv[0]);
	}
}

