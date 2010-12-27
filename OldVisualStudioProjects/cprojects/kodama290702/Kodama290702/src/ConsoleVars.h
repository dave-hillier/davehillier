#ifndef __CONSOLEVARS_H__
#define __CONSOLEVARS_H__

#define MAX_VARS 256

class ConsoleVar {
private:
	float value;

public:
	int key; // TODO should be encapsulated
	// TODO Write variables/settings

	char *name;
	char *string;	// TODO needs destructor for deletion
	int flags;

	void Set(char *nme, char *str, int flgs = 0);
	inline float Value() {return value;};
	ConsoleVar () { name = NULL; string = NULL; key = -1; flags = 0; };
	virtual ~ConsoleVar ();
};

class ConsoleVars {
public:
	ConsoleVars();
	virtual ~ConsoleVars();

	ConsoleVar *Add(char *name, char *str, int flgs = 0); // TODO  use flags for something
	ConsoleVar *Get(char *name);

private:
	int used, size;
	ConsoleVar *vars[MAX_VARS];

};
extern ConsoleVars *cvars;

#endif