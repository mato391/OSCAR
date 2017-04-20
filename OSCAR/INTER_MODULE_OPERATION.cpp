#include "stdafx.h"
#include "INTER_MODULE_OPERATION.h"


INTER_MODULE_OPERATION::INTER_MODULE_OPERATION(std::string name, std::string details)
{
	this->operation = name;
	this->details = details;
}


INTER_MODULE_OPERATION::~INTER_MODULE_OPERATION()
{
}
