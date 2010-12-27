// ConsoleAlias.cpp: implementation of the ConsoleAlias class.
//		Its actually just a slightly altered version of the Vars class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Console.h"
#include "ConsoleCmds.h"
#include "ConsoleAlias.h"

int MD4Data (char *buf, int len);
ConsoleAliases *alias;
void CmdExecuteAlias (int argc, char *argv[]);

void ConsoleAlias::Set(char *nme, char *str)
{
	int len = strlen(nme);

	if (name)
		delete name;

	name = new char[len]; 

	for (int i = 0; i < len; i++)
		name[i] = tolower(nme[i]);
	name[i] = 0;

	key = MD4Data(name, i);

	len = strlen(str);
	string = new char[len];
	strcpy(string, str);

}


ConsoleAlias::~ConsoleAlias ()
{
	if (name)
		delete name;
	if (string)
		delete string;
}

ConsoleAliases::ConsoleAliases ()
{
	used = 0;
	size = MAX_ALIASES;

	// set all pointers to 0
	for (int i=0; i < MAX_ALIASES; i++)
		aa[i] = NULL;
}
ConsoleAliases::~ConsoleAliases()
{
	for (int i=0; i < MAX_ALIASES; i++)
	{
		if (aa[i] != NULL)
			delete aa[i];
	}
}

ConsoleAlias *ConsoleAliases::Get(char *name)
{
	int i;
	ConsoleAlias *ca = new ConsoleAlias();
	ca->Set(name, "a");

	int index = abs(ca->key % size);
	i = index;

	while (true)
	{

		if (aa[i] && aa[i]->key == ca->key)
		{
			delete ca;
			return aa[i];
		}
		else
		{
			i = (i+1) % size;

		}

		if (i == index)
			return NULL;
	}
}

ConsoleAlias *ConsoleAliases::Add(char *name, char *str)
{
	int i;
	ConsoleAlias *cv = new ConsoleAlias();
	cv->Set(name, str);

	int index = abs(cv->key % size);
	
	if (used >= size-1)
		return NULL;

	used++;

	i = index;

	// probe, find next free!
	while (true)
	{

		if (aa[i] && aa[i]->key == cv->key)
		{
			int len = strlen(aa[i]->string);
			delete aa[i]->string;
			aa[i]->string = new char[len];
			strcpy(aa[i]->string, str);

			// update value
			return aa[i];
		}
		else if (aa[i])
		{
			i = (i+1) % size;
		}
		else 
		{
			aa[i] = cv;
			break;
		}
	}

	command->Add(name, CmdExecuteAlias);


	return cv;
}

void CmdAlias (int argc, char *argv[])
{
	if (argc >= 3)
	{
		// concat args >= 2
		char args[128];
		args[0] = 0;

		for (int i = 2; i < argc; i++)
		{
			strcat(args, argv[i]);
			strcat(args, " ");
		}

		ConsoleAlias *ca = alias->Get(argv[1]);

		if (!ca)
		{
			alias->Add(argv[1], args);
		}
		else
			ca->Set(argv[1], args);
	}
	else if (argc == 2)
	{
		ConsoleAlias *ca = alias->Get(argv[1]);
		if (ca)
			con->Printf("\"%s\" = \"%s\"\n", ca->name, ca->string);
		else
			con->Printf("%s is not set\n", argv[1]);

	}
	else
	{
		// list all aliases?
	}
}

void CmdExecuteAlias (int argc, char *argv[])
{
	ConsoleAlias *ca = alias->Get(argv[0]);

	// TODO prevent infinite loops


	if (ca)
		command->Execute(ca->string);	

}

