// EventScheduler.cpp: implementation of the EventScheduler class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ConsoleCmds.h"
#include "SDLTimer.h"
#include "EventScheduler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
EventScheduler *events;

EventScheduler::~EventScheduler()
{
	// delete all pending events
}

void EventScheduler::Update ()
{
	CmdEvent *ev, *temp = NULL; 

	for (ev = eventlist; ev != NULL;/* ev = ev->next*/)
	{
		if (ev->time > timer.GetTime())
			break; // should be in order, dont check events to happen later!
		else
		{
			command->Execute(ev->cmd);
			temp = ev->next;

			// unlink and delete ev
			if (ev->next)
				ev->next->prev = ev->prev;
			if (ev->prev)
				ev->prev->next = ev->next;

			if (eventlist == ev)
				eventlist = ev->next;

	//		delete ev->cmd;
	//		delete ev;
			ev = temp;

			// set the next event in the chain if possible

		}
		
	}
}

void EventScheduler::AddEvent(char *str, float time)
{
	time += timer.GetTime();
	CmdEvent *ev, *adding; 

	if (!eventlist)
	{
		adding = new CmdEvent;
		adding->next = eventlist;
		adding->prev = NULL;
		adding->time = time;

		adding->cmd = new char[strlen(str)];
		strcpy(adding->cmd, str);
		eventlist = adding;
		return;
	}

	for (ev = eventlist; ev != NULL; ev = ev->next)
	{
		if (time <= ev->time) // should insert in order
		{
			// insert before
			adding = new CmdEvent;
			adding->next = ev;
			adding->time = time;
			adding->prev = ev->prev;
			adding->cmd = new char[strlen(str)];

			strcpy(adding->cmd, str);

			if (ev == eventlist)
				eventlist = adding;

			if (ev->prev)
				ev->prev->next = adding;

			ev->prev = adding;

			break;
		}
		else if (!ev->next)
		{
			adding = new CmdEvent;
			adding->next = NULL;
			adding->time = time;
			adding->prev = ev;
			adding->cmd = new char[strlen(str)];

			strcpy(adding->cmd, str);

			ev->next = adding;
			break;
		}
	}

}

void CmdEvent (int argc, char *argv[])
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
		events->AddEvent(args, (float)atof(argv[1]));
	}
}