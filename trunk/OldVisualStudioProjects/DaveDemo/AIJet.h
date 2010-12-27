// AIJet.h: interface for the PlayerJet class.
//
//////////////////////////////////////////////////////////////////////
#include "Jet.h"

class AIJet  
{
public:
	AIJet();
	virtual ~AIJet();

	void Update ();
	Jet *jet;
};

