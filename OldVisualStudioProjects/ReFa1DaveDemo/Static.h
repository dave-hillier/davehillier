// Static.h: interface for the Static class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __STATIC__
#define __STATIC__
#include "new_src/Actor.h"
class Static : public Actor
{
public:
	virtual void Update();
	virtual void Render();
	Static(int t = 0);
	virtual ~Static();

	int type;

};
#endif // __Static__