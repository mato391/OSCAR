#pragma once
#include "Objects\Obj.hpp"
#include <vector>
#include "Objects\LIGHT.hpp"
#include "CONNECTOR.hpp"
class POWER_GROUP :
	public Obj
{
public:
	POWER_GROUP(std::string label);
	~POWER_GROUP();
	std::string label;
	std::vector<LIGHT*> lights_;
	CONNECTOR* commonGND;
};

