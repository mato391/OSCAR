#pragma once
#include "Objects\Obj.hpp"
class SWITCH_CONNECTOR :
	public Obj
{
public:
	SWITCH_CONNECTOR();
	~SWITCH_CONNECTOR();
	int id;
	std::string label;
	int value;
};

