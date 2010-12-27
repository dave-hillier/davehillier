
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "Console.h"
#include "sdltimer.h"
#include "camera.h"
#include "ConsoleCmds.h"
#include "ConsoleVars.h"

int MD4Data (char *buf, int len);
// TODO cvar list, command list, print with cvar value insertion
// configuration file loading

ConsoleCmds *command;

/*
	not brilliantly object orientated, but commands could be part of 
	other obects, or if we need to have a separate object containing 
	all the	commands.
*/
void CmdClear (int argc, char *argv[])
{
	con->Clear();
}
void CmdFPS (int argc, char *argv[])
{
	con->Printf("%f\n", 1/timer.Delta());
}
void CmdPos (int argc, char *argv[])
{
	Vector v = camera->GetPosition();
	con->Printf("(%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);
}

void CmdTest (int argc, char *argv[])
{
	if (argc == 1)
	{
		con->Printf("Test <arg1> <arg2> etc\n");
		return;
	}
	
	for (int i = 1; i < argc; i++)
	{
		con->Printf("%d: %s\n", i, argv[i]);
	}
}
void CmdList (int argc, char *argv[])
{
	command->ListAll();
}
void CmdSet (int argc, char *argv[]);
void CmdSet2 (int argc, char *argv[]);
void CmdBind(int argc, char *argv[]);
void CmdEvent (int argc, char *argv[]);
void CmdQuit (int argc, char *argv[]); 
void CmdAlias (int argc, char *argv[]);
void CmdTerrain (int argc, char *argv[]);

// camera.cpp
void CmdLookLeftDown(int argc, char *argv[]);
void CmdLookLeftUp(int argc, char *argv[]);
void CmdLookRightDown(int argc, char *argv[]);
void CmdLookRightUp(int argc, char *argv[]);
void CmdLookUpDown(int argc, char *argv[]);
void CmdLookUpUp(int argc, char *argv[]);
void CmdLookDownDown(int argc, char *argv[]);
void CmdLookDownUp(int argc, char *argv[]);

void CmdMoveForwardDown(int argc, char *argv[]);
void CmdMoveForwardUp(int argc, char *argv[]);
void CmdMoveBackDown(int argc, char *argv[]);
void CmdMoveBackUp(int argc, char *argv[]);
void CmdMoveRightDown(int argc, char *argv[]);
void CmdMoveRightUp(int argc, char *argv[]);
void CmdMoveLeftDown(int argc, char *argv[]);
void CmdMoveLeftUp(int argc, char *argv[]);

ConsoleCmds::ConsoleCmds()
{
	used = 0;
	size = MAX_CMDS;

	for (int i =0; i < MAX_CMDS; i++)
	{
		cmds[i].key = -1;
	}


	Add("alias", CmdAlias);
	Add("after", CmdEvent);
	Add("bind", CmdBind);
	Add("clear", CmdClear);
	Add("quit", CmdQuit);
	Add("set", CmdSet);
	Add("test", CmdTest);
	Add("listcommands", CmdList);
	Add("map", CmdTerrain);
	Add("fps", CmdFPS);
	Add("pos", CmdPos);


	Add("+lookleft", CmdLookLeftDown);	
	Add("-lookleft", CmdLookLeftUp);
	Add("+lookright", CmdLookRightDown);
	Add("-lookright", CmdLookRightUp);
	Add("+lookup", CmdLookUpDown);
	Add("-lookup", CmdLookUpUp);
	Add("+lookdown", CmdLookDownDown);
	Add("-lookdown", CmdLookDownUp);
	
	Add("+fwd", CmdMoveForwardDown); // for some reason +forward causes a crash
										// but not while in debugger? havent got a clue
	Add("-fwd", CmdMoveForwardUp);	
	Add("+back", CmdMoveBackDown);
	Add("-back", CmdMoveBackUp);

	Add("+left", CmdMoveLeftDown);
	Add("-left", CmdMoveLeftUp);
	Add("+right", CmdMoveRightDown);
	Add("-right", CmdMoveRightUp);
	


}

void ConsoleCmds::Add (char *nme, void (*func)(int argv, char *argc[]))
{
	int key = MD4Data(nme, strlen(nme));
	int index = abs(key % size);


	
	if (used >= size-1)
		return;

	used++;

	// probe!
	while (true)
	{
		if (cmds[index].key == -1)
			break;
		else
			index = (index+1) % size;
	}

	cmds[index].key = key;
	cmds[index].func = func;

	int len = strlen(nme);
	len += len %4;
	cmds[index].name = new char[len];
	strcpy(cmds[index].name, nme);
}

void ConsoleCmds::Execute (char *cmd)
{
	bool more = true;
	// parse here
	char *ch = cmd;

	// for each line
	if (strlen(cmd) == 0)
		return;

	do
	{
		int argc = 0, i = 0;
		bool quotes = false;
		char *argv[MAX_ARGS];
		char temp[100];

		temp[0] = 0;

		// count the tokens
		// copy tokens into argv

		while (true)
		{
			if (*ch == 0)
			{
				more = false;
				break;
			}
			else if (*ch == '\n')
			{
				ch++;
				break; 
			}
			else if (*ch == '"')
			{
				quotes = !quotes;
			}
			else if ((*ch == ';') && !quotes)
			{
					ch++;
				//	temp[0] = 0;
					break; 
			}
			else if ((*ch == ' ' ) && !quotes) // check for quotes
			{
				if (strlen(temp) > 0)
				{
					argv[argc] = new char[strlen(temp)+1];
					strcpy (argv[argc], temp);
					argc++;
					temp[0] = 0;
					i = 0;

				}
			}
			else
			{
				temp[i] = *ch;
				temp[++i] = 0;
			}
			ch++;
		}
		if (strlen(temp) > 0)
		{
			argv[argc] = new char[strlen(temp)+1];
			strcpy (argv[argc], temp);
			argc++;
			temp[0] = 0;
		}

		for (i = 0; i < (signed)strlen(argv[0]); i++)
			temp[i] = tolower(argv[0][i]);
		temp[i+1] = 0;
		
		int key = MD4Data(temp, i); //MD4Data((argv[0]), strlen(argv[0]));
		int index = abs(key % size);

		i = index;
		// probe
		while (true)
		{
			if (cmds[i].key == key)
			{
				// got it!
				cmds[i].func(argc, argv);
				break;
			}
			else
			{
				i = (i+1) % size;
				if (i == index)
				{
					con->Printf("Unknown command: %s\n", argv[0]);
					break;//return;
				} 
		
			}

		} // while (true)
		for (i = 0; i < argc; i++)
		{
			delete argv[i];
		}			
	} 	while (more);

}

void ConsoleCmds::ListAll ()
{
	for (int i=0; i < size; i++)
	{
		if (cmds[i].key != -1)
		{

			con->Printf("%s\n", cmds[i].name);
		}
	}
}

void ConsoleCmds::ReadSettings (char *cfgname)
{
	FILE *fp;
	char line[256];
	unsigned  int i;

	if (fp = fopen(cfgname, "r"))
	{
		con->Printf("Executing %s...\n", cfgname);
		while (!feof(fp))
		{
			fgets(line, 256, fp);
			for (i=0; i < strlen(line); i++)
				if (line[i] == '\n')
					line[i] = 0;
			// remove whitespace
			if (strlen(line) > 1)
				Execute(line);
		}
		fclose(fp);
	}
	else
		con->Printf("Error opening %s\n", cfgname);	
}
