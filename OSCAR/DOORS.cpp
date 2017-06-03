#include "stdafx.h"
#include "DOORS.hpp"


DOORS::DOORS()
{
	name = "DOORS";
}


DOORS::~DOORS()
{
}

void DOORS::addDoors(DOOR* door)
{
	container_.push_back(door);
}

void DOORS::setLockingState(int state)
{
	for (auto &door : container_)
	{
		door->lockingState = static_cast<DOOR::ELockingState>(state);
	}
}