// Overlay.h: interface for the Overlay class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __OVERLAY__
#define __OVERLAY__

class Overlay  
{
public:
	Overlay();
	virtual ~Overlay();

	void Display();
	void Update();
	float vel;
	float health;
};

extern Overlay overlay;

#endif // __OVERLAY__