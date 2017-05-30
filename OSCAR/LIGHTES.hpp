#pragma once
#include "Objects\Obj.hpp"
#include "Objects\LIGHT.hpp"
#include <vector>
#include "POWER_GROUP.hpp"
class LIGHTES :
	public Obj
{
public:
	LIGHTES();
	~LIGHTES();
	void addLight(LIGHT* light);
	std::vector<POWER_GROUP*> powerGroups_;

};

