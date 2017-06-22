#include "stdafx.h"
#include "commonGNDChecker.hpp"


commonGNDChecker::commonGNDChecker()
{
}


commonGNDChecker::~commonGNDChecker()
{
}

bool commonGNDChecker::checkGNDConnVelue(CONNECTOR* conn)
{
	if (conn->label.find("GND") != std::string::npos && conn->value != 0 )
	{
		return false;
	}
	return true;
}
