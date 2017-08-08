#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
class CHANGE_BUTTON_STATE_IND :
	public Obj
{
public:
	CHANGE_BUTTON_STATE_IND();
	~CHANGE_BUTTON_STATE_IND();
	std::string buttonLabel;
	int value;
};

