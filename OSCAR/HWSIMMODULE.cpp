#include "stdafx.h"
#include "HWSIMMODULE.hpp"


HWSIMMODULE::HWSIMMODULE(std::string name, int ownid)
{
	this->name = name;
	this->ownId = ownId;
	can = new CAN(this->ownId);
}


HWSIMMODULE::~HWSIMMODULE()
{
}
