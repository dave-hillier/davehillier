// Static.h: interface for the Static class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __STATIC__
#define __STATIC__
#include "Unit.h"
class Static : public Unit
{
public:
	virtual void Update();
	virtual void Render();
	Static(int t = 0);
	virtual ~Static();

	int type;

};
#endif // __Static__