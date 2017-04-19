#include "stdafx.h"
#include "LIGHTES.hpp"


LIGHTES::LIGHTES()
{
	name = "LIGHTES";
}


LIGHTES::~LIGHTES()
{
}

void LIGHTES::addLight(LIGHT* light)
{
	container_.push_back(light);
}
