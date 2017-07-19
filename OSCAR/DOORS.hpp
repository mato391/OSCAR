#pragma once
#include "Objects\Obj.hpp"
#include "Objects\DOOR.hpp"
#include <vector>
#include "PORT.hpp"
class DOORS :
	public Obj
{
public:
	DOORS();
	~DOORS();
	void addDoors(DOOR* door);
	std::vector<DOOR*> container_;
	PORT* commonLockGND;
	void setLockingState(int state);
	enum class EOpeningState
	{
		closed,
		opened
	};
	EOpeningState openingState;
	enum class ELockingState
	{
		locked,
		unlocked
	};
	ELockingState lockingState;
};

