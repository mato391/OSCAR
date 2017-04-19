#pragma once
#include "Objects\Obj.hpp"
#include <vector>
#include <iostream>
class REMOTE_CONTROLLER :
	public Obj
{
public:
	REMOTE_CONTROLLER(std::string serialNumber);
	~REMOTE_CONTROLLER();
	std::string serialNumber;
	bool login();
private:
	const std::vector<std::string> rcValidSNVec = { "9999", "9998" };
};

