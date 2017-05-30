#pragma once
#include "Objects\Obj.hpp"
#include <vector>
#include "Objects\LIGHT.hpp"
class POWER_GROUP :
	public Obj
{
public:
	POWER_GROUP(std::string label);
	~POWER_GROUP();
	std::string label;
	std::vector<LIGHT*> lights_;
};

