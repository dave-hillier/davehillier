// font.h: interface for the font class.
//
//////////////////////////////////////////////////////////////////////


#ifndef __FONT__
#define __FONT__
class Font  
{
public:
	void printf(char *format, ...);
	Font(char *name);
	virtual ~Font();
private:
	int base;
};

extern Font *arial;
#endif //__FONT__
