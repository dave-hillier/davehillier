#include "Joystick.h"
#include "DebugLog.h"

#include <Windows.h>

Joystick::Joystick(void)
{
	unsigned int numJoysticks = joyGetNumDevs();
	JOYINFOEX info;
	MMRESULT result = joyGetPosEx(0, &info);

}

Joystick::~Joystick(void)
{
}

void Joystick::update(double timeNow)
{
	JOYINFOEX info;
	MMRESULT result = joyGetPosEx(0, &info);

	axis_[0] = (((int)info.dwXpos) - 32767) / (32767.f) ;
	axis_[1] = (((int)info.dwYpos) - 32767) / (32767.f) ;
	axis_[2] = (((int)info.dwZpos) - 32767) / (32767.f) ;
	axis_[3] = (((int)info.dwRpos) - 32767) / (32767.f) ;
	

	// TODO Dead zone? seems to have one already!
	buttons_[0] = (info.dwButtons & JOY_BUTTON1) != 0;
	buttons_[1] = (info.dwButtons & JOY_BUTTON2) != 0;
	buttons_[2] = (info.dwButtons & JOY_BUTTON3) != 0;
	buttons_[3] = (info.dwButtons & JOY_BUTTON4) != 0;
	buttons_[4] = (info.dwButtons & JOY_BUTTON5) != 0;
	buttons_[5] = (info.dwButtons & JOY_BUTTON6) != 0;
	buttons_[6] = (info.dwButtons & JOY_BUTTON7) != 0;
	buttons_[7] = (info.dwButtons & JOY_BUTTON8) != 0;
	buttons_[8] = (info.dwButtons & JOY_BUTTON9) != 0;
	buttons_[9] = (info.dwButtons & JOY_BUTTON10) != 0;


	// TODO DPad
}
