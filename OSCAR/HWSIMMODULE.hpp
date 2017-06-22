#pragma once
#include <string>
#include "CAN.h"
class HWSIMMODULE
{
public:
	HWSIMMODULE(std::string name, int ownid);
	~HWSIMMODULE();
	std::string name;
	int ownId;
	CAN* can;
};

