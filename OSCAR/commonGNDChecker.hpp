#pragma once
#include "CONNECTOR.hpp"
class commonGNDChecker
{
public:
	commonGNDChecker();
	~commonGNDChecker();
	static bool checkGNDConnVelue(CONNECTOR* conn);
};

