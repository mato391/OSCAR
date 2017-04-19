#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
class BUTTON :
	public Obj
{
public:
	BUTTON();
	~BUTTON();
	std::string label;
	std::string port;
	void turnOnLed() {};
};

