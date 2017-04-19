#include "stdafx.h"
#include "REMOTE_CONTROLLER.hpp"


REMOTE_CONTROLLER::REMOTE_CONTROLLER(std::string serialNumber)
{
	name = "REMOTE_CONTROLLER";
	this->serialNumber = serialNumber;
}


REMOTE_CONTROLLER::~REMOTE_CONTROLLER()
{
}

bool REMOTE_CONTROLLER::login()
{
	for (const auto &sn : rcValidSNVec)
	{
		if (sn == serialNumber)
		{
			std::cout << "REMOTE_CONTROLLER::login: passed" << std::endl;
			return true;
		}
			
	}
	std::cout << "REMOTE_CONTROLLER::login: failed" << std::endl;
	return false;
}
