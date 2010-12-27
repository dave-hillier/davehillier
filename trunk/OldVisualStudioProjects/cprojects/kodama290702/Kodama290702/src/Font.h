// Font.h: interface for the Font class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __FONT_H__
#define __FONT_H__
class Font  
{
public:
	Font(char *name, int c, int r, int s);
	virtual ~Font();
	void RenderChar (int ch, int x, int y);
	void Printf(int x, int y, char *format, ...);

	int GetSize() { return size; };
	void SetSize(int s) { size = s; };

private:
	float tex_coords[256][2];
	int tex_id;
	int cols, rows, size;
};

#endif