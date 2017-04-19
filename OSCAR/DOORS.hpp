#pragma once
#include "Objects\Obj.hpp"
#include "Objects\DOOR.hpp"
#include <vector>
class DOORS :
	public Obj
{
public:
	DOORS();
	~DOORS();
	void addDoors(DOOR* door);
	std::vector<DOOR*> container_;
};

