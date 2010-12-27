// EventScheduler.h: interface for the EventScheduler class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __EVENT_H__
#define __EVENT_H__

class CmdEvent {

public:
	float time;
	char *cmd;
	CmdEvent *next, *prev;
};

class EventScheduler  
{
public:
	EventScheduler() { eventlist = NULL; };
	virtual ~EventScheduler();

	void AddEvent(char *str, float time); 

	void Update ();
private:
	// insert?
	CmdEvent *eventlist;
};

extern EventScheduler *events;

#endif