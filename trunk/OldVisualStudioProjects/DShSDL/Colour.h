#ifndef _Colour_h
#define _Colour_h

class Colour
{
public:
	Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : 
	  r_(r), g_(g), b_(b), a_(a)
	{
	}

	Colour(const Colour &c) : 
	r_(c.r_), g_(c.g_), b_(c.b_), a_(c.a_)
	{
	}

	unsigned char r() const { return r_; }
	unsigned char g() const { return g_; }
	unsigned char b() const { return b_; }
	unsigned char a() const { return a_; }
	void r(unsigned char c) { r_ = c; }
	void g(unsigned char c) { g_ = c; }
	void b(unsigned char c) { b_ = c; }
	void a(unsigned char c) { a_ = c; }

private:
	unsigned char r_;
	unsigned char g_; 
	unsigned char b_; 
	unsigned char a_;
};

#endif //_Colour_h