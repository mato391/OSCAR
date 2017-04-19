#pragma once
#include "Objects/Obj.hpp"
class PEDAL :
	public Obj
{
public:
	PEDAL();
	~PEDAL();
	std::string label;
	std::string port;
	bool isPushed;
};

