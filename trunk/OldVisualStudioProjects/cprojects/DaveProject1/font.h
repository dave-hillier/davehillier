// font.h: interface for the font class.
//
//////////////////////////////////////////////////////////////////////


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