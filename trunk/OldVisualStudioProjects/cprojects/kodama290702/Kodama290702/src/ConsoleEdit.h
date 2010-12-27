// ConsoleEdit.h: interface for the ConsoleEdit class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __CONSOLEEDIT_H__
#define __CONSOLEEDIT_H__

#define CON_BUFFER_SIZE 200
#define CON_LINE_WIDTH 256

class ConsoleEdit  
{
public:
	ConsoleEdit();
	virtual ~ConsoleEdit();
	void Render ();
	bool KeyDown (int key);
	void KeyUp (int key);

	void CompletePartial (char *ptr);
	int ListCommand (bool print = false);

private:
	void HistoryAddLine (char *str);
	char *HistoryGetLine ();

	int repeatkey;
	float repeatkeytime;
	int cursor;
	bool insert;
	char buffer[CON_LINE_WIDTH];

	char keyshift[400];

	// command history buffer
	char history[CON_BUFFER_SIZE][CON_LINE_WIDTH];
	int head, tail;
	int history_back_scroll; // for fetching previous commands

	float cursor_blink;
	bool cursor_visible;

	bool tabbed;

};

extern ConsoleEdit *cedit;

#endif