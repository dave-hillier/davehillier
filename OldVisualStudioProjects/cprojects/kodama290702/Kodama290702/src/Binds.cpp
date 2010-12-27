#include <stdio.h>
#include <string.h>
#include "Console.h"
#include "ConsoleCmds.h"
#include "Input.h"
#include "Binds.h"

Binds *bind;
Binds::Binds()
{
	for (int i = 0; i < NUM_KEYS; i++)
	{
		cmdstr[i] = NULL;
	}
}

Binds::~Binds()
{
	for (int i = 0; i < NUM_KEYS; i++)
	{
		if (cmdstr[i])
			delete cmdstr[i];
	}
}

void Binds::Set(int key, char *str)
{
	if (cmdstr[key])
		delete cmdstr[key];

	cmdstr[key] = new char[strlen(str)];
	strcpy(cmdstr[key], str);
}

void Binds::Execute(int key, bool release)
{
	if (!cmdstr[key])
		return;

	if (release && cmdstr[key][0] =='+')
	{
		cmdstr[key][0] ='-';
		command->Execute(cmdstr[key]);
		cmdstr[key][0] ='+';
	}
	else if (!release)
		command->Execute(cmdstr[key]);
}

void CmdBind(int argc, char *argv[])
{
	if (argc == 3)
	{
		int code = input->Code(argv[1]);
		if (code)
			bind->Set(code, argv[2]);
		else
			con->Printf("Unknown key: %s\n",argv[1]);

	}
	else if (argc == 2)
	{
		int code = input->Code(argv[1]);
		// print what it is
		if (code)
			con->Printf("\"%s\" = \"%s\"\n", argv[1], bind->cmdstr[code]);
		else
			con->Printf("Unknown key: %s\n",argv[1]);
	}
	else
	{
		con->Printf("Bind <key> <command>\n");
	}
}
