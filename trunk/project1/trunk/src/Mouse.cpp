#include "Mouse.h"
#include "DebugLog.h"

#include <Windows.h>


Mouse::Mouse() : x_(0), y_(0), lastX_(0), lastY_(0), 
	left_(false), middle_(false), right_(false)
{

}

Mouse::~Mouse()
{
}

void Mouse::visible(bool vis)
{
	ShowCursor(vis);
}


void Mouse::update(double timeNow)
{
	// Polling, not efficient but easy
	lastX_ = x_;
	lastY_ = y_;

	POINT point;
	GetCursorPos(&point);

	x_ = point.x;
	y_ = point.y;
	
	left_ = GetAsyncKeyState( VK_LBUTTON ) != 0;
	middle_ = GetAsyncKeyState( VK_MBUTTON ) != 0;
	right_ = GetAsyncKeyState( VK_RBUTTON ) != 0;

	// 5 & 6 for forward and back buttons
}

