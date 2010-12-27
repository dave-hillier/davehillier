//#include "keycodes.h"
#ifndef __BINDS_H__
#define __BINDS_H__

class Binds {
public:
	Binds ();
	virtual ~Binds ();

	// add delete/unbind
	void Set(int, char *);
	void Execute(int key, bool release = false);
	char *cmdstr[NUM_KEYS];
};

extern Binds *bind;
#endif