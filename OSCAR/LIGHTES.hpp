#pragma once
#include "Objects\Obj.hpp"
#include "Objects\LIGHT.hpp"
#include <vector>

class LIGHTES :
	public Obj
{
public:
	LIGHTES();
	~LIGHTES();
	void addLight(LIGHT* light);
	std::vector<LIGHT*> container_;

};

