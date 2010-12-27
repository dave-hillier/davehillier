// ConsoleAlias.h: interface for the ConsoleAlias class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __CONSOLEALIAS_H__
#define __CONSOLEALIAS_H__

#define MAX_ALIASES 256

class ConsoleAlias {
private:

public:
	int key; // should be encapsulated

	char *name;
	char *string;	// TODO needs destructor for deletion

	void Set(char *nme, char *str);
	ConsoleAlias () { name = NULL; string = NULL; key = -1;	};
	virtual ~ConsoleAlias ();
};

class ConsoleAliases {
public:
	ConsoleAliases();
	virtual ~ConsoleAliases();

	ConsoleAlias *Add(char *name, char *str); // TODO use flags for something
	ConsoleAlias *Get(char *name);

private:
	int used, size;
	ConsoleAlias *aa[MAX_ALIASES];
};

extern ConsoleAliases *alias;

#endif // __CONSOLEALIAS_H__