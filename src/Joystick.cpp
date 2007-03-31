#include "Joystick.h"
#include "DebugLog.h"

#include <Windows.h>
#include <cassert>

Joystick::Joystick(unsigned int pad) : pad_(pad)
{

}

Joystick::~Joystick()
{
}

bool Joystick::update(double timeNow)
{
	JOYINFOEX info;
	MMRESULT result = joyGetPosEx(1, &info);

	if (JOYERR_NOERROR == result)
	{
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
		buttons_[10] = (info.dwButtons & JOY_BUTTON11) != 0;
		buttons_[11] = (info.dwButtons & JOY_BUTTON12) != 0;

		// DPad
		buttons_[12] = info.dwPOV != JOY_POVCENTERED && (info.dwPOV >= JOY_POVFORWARD && info.dwPOV < JOY_POVRIGHT) || info.dwPOV > JOY_POVLEFT;// Up
		buttons_[13] = info.dwPOV != JOY_POVCENTERED && info.dwPOV > JOY_POVFORWARD && info.dwPOV < JOY_POVBACKWARD;// Right
		buttons_[14] = info.dwPOV != JOY_POVCENTERED && info.dwPOV > JOY_POVRIGHT && info.dwPOV < JOY_POVLEFT;// Down
		buttons_[15] = info.dwPOV != JOY_POVCENTERED && info.dwPOV > JOY_POVBACKWARD;// Left
	}
	else
	{
		debugLog << "Error " << result << " for pad " << pad_ <<  std::endl;
	}

	return (JOYERR_NOERROR == result);
}
