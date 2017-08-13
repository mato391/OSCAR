#pragma once
#include "Obj.hpp"
#include "WINDOW.hpp"
#include "../PORT.hpp"
#include <iostream>
#include <vector>
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
	std::vector<int> refs;
	void unlockDoor();
	void lockDoor();
	void openDoor();
	void closeDoor();
	void changeConnectorState(int connId, int value);
};

