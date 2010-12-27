// PlayerJet.cpp: implementation of the PlayerJet class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "Input.h"
#include "Timer.h"

#include "Jet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PlayerJet::PlayerJet()
{
	rudder = 0;
	pitch_control = 0;
	power = 1;
}

PlayerJet::~PlayerJet()
{

}

void PlayerJet::Update ()
{
	rudder -= input.mouse_x * 0.25f;

	if (rudder > 75)
		rudder = 75;
	else if (rudder < -75)
		rudder = -75;

	if (rudder > 1)
		rudder -= 20 * timer.Delta();
	else if (rudder < -1)
		rudder += 20 * timer.Delta();

	pitch_control += input.mouse_y * 0.5f;

	if (pitch_control > 1)
		pitch_control -= 30 * timer.Delta();
	else if (pitch_control < -1)
		pitch_control += 30 * timer.Delta();

	if (power < 2)
		power += timer.Delta() * input.State(VK_MBUTTON);

	if (power > 0.5)
		power -= 0.5f * timer.Delta();


	// try get a target
	
	if (input.State(VK_RBUTTON))
		fire1 = true;
	else
		fire1 = false;

	// just fire blaster
	if (input.State(VK_LBUTTON))
		fire2 = true;
	else
		fire2 = false;


}

