#pragma once
#include "Objects\Obj.hpp"
class SWITCH :
	public Obj
{
public:
	SWITCH();
	~SWITCH();
	std::string label;
	std::vector<int> refs;
	int pos;
};

