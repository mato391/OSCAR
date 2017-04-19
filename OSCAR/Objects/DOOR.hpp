#pragma once
#include "Obj.hpp"
#include "WINDOW.hpp"
#include <iostream>
class DOOR : public Obj
{
public:
	DOOR(std::string label);
	~DOOR();
	std::string label;
	enum class ELockingState
	{
		locked,
		unlocked
	};
	ELockingState lockingState;
	enum class EOpeningState
	{
		closed,
		opened
	};
	EOpeningState openingState;
	WINDOW* window;
	void unlockDoor();
	void lockDoor();
	void openDoor();
	void closeDoor();
};

