#include "stdafx.h"
#include "EQM.hpp"


EQM::EQM()
{
	name = "EQM";
}


EQM::~EQM()
{
}

void EQM::addModule(Obj* obj)
{
	modules_.push_back(obj);
}
