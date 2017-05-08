#include "stdafx.h"
#include "CONNECTOR.hpp"


CONNECTOR::CONNECTOR(int id)
{
	name = "CONNECTOR";
	this->id = id;
	used = false;
}


CONNECTOR::~CONNECTOR()
{
}
