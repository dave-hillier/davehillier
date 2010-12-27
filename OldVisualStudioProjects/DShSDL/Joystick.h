#pragma once

struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;

class Joystick
{
public:
	Joystick(int port);
	~Joystick();

	static void Update();
	float AxisLeftX() const;
	float AxisLeftY() const;
	float AxisRightX() const;
	float AxisRightY() const;

	bool Button(int button) const;

private:
	int port_;
	SDL_Joystick *joy_;

	float Axis(int i) const; 
};
