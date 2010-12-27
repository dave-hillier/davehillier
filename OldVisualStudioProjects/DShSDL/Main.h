#pragma once

struct SDL_Surface;

class Main
{
public:
	Main();
	~Main();

	bool Loop();
private:
	void Update();
	void Draw();
	SDL_Surface *pScreen_;

	const unsigned int width_;
	const unsigned int height_;
	bool fullscreen_;
	unsigned char* keys_;

	bool CreateWindowGL(bool fullscreen);
};
