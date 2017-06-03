#include "stdafx.h"
#include "DOOR.hpp"


DOOR::DOOR(std::string label)
{
	name = "DOOR";
	window = new WINDOW();
	if (label != "MASK" && label != "BOOT")
	{
		window->openable = true;
	}
	this->label = label;
	window->openable = false;
	window->label = label;
	window->lockingState = WINDOW::ELockingState::unlocked;
}


DOOR::~DOOR()
{
	delete window;
}

void DOOR::unlockDoor()
{
	lockingState = DOOR::ELockingState::unlocked;
	for (auto &port : ports)
	{
		for (auto &conn : port->connectors)
		{
			if (conn->type == CONNECTOR::EType::output)
			{
				conn->value = 0;
			}
		}
	}
}

void DOOR::lockDoor()
{
	lockingState = DOOR::ELockingState::locked;
	window->lockingState = WINDOW::ELockingState::locked;
	for (auto &port : ports)
	{
		for (auto &conn : port->connectors)
		{
			if (conn->type == CONNECTOR::EType::output)
				conn->value = 1;	
		}
	}
}

void DOOR::openDoor()
{
	openingState = DOOR::EOpeningState::opened;
	for (auto &port : ports)
	{
		for (auto &conn : port->connectors)
		{
			if (conn->type == CONNECTOR::EType::input)
				conn->value = 0;
		}
	}
}

void DOOR::closeDoor()
{
	openingState = DOOR::EOpeningState::closed;
	for (auto &port : ports)
	{
		for (auto &conn : port->connectors)
		{
			if (conn->type == CONNECTOR::EType::input)
				conn->value = 1;
		}
	}
}
