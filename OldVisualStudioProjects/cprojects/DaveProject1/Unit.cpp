// Unit.cpp: implementation of the Unit class.
// Should be an abstract class for objects
//////////////////////////////////////////////////////////////////////
#include <stdio.h>

#include "Unit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static Unit *uhead = NULL;
Unit::Unit()
{
	next = uhead;
	if (next)
		next->prev = this;

	prev = NULL;
	uhead = this;
}

Unit::~Unit()
{

	
	if (this == uhead)
		uhead = next;	
	
	if (next)
		next->prev = prev;

	if (prev)
		prev->next = next;
}


void UManager::Delete (Unit *u)
{
	if (u == uhead)
	{
		uhead = u->next;
		uhead->prev = NULL;
		delete u;
	}
	else if (u->next == NULL)
	{
		if (u->prev)
			u->prev->next = NULL;

		delete u;
	}
	else
	{
		u->prev->next = u->next;
		u->next->prev = u->prev;
		delete u;
	}
}

void UManager::Render ()
{
	Unit *u;
	for (u = uhead; u; u = u->next)
	{
		u->Render();
	}
}
void UManager::Update ()
{
	Unit *u;
	for (u = uhead; u; u = u->next)
	{
		u->Update();
	}
}

UManager uman;
// TODO: add model and particle display functions