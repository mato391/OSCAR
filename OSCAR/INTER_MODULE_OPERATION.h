#pragma once
#include <string>

class INTER_MODULE_OPERATION
{
public:
	INTER_MODULE_OPERATION(std::string name, std::string details);
	~INTER_MODULE_OPERATION();
	std::string operation;
	std::string details;
};

