#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
class ALARM :
	public Obj
{
public:
	ALARM();
	ALARM(std::string description, int alarmCode, std::string ref);
	~ALARM();
	std::string description;
	int alarmCode;
	std::string ref;
};

