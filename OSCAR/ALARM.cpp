#include "stdafx.h"
#include "ALARM.hpp"

ALARM::ALARM()
{
	name = "ALARM";
}

ALARM::ALARM(std::string description, int alarmCode, std::string ref)
{
	name = "ALARM";
	this->description = description;
	this->alarmCode = alarmCode;
	this->ref = ref;
}


ALARM::~ALARM()
{
}
