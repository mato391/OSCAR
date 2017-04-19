#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
#include <vector>
#include "BUTTON.hpp"
#include "PEDAL.h"

class SWITCH_TOPOLOGY :
	public Obj
{
public:
	SWITCH_TOPOLOGY();
	~SWITCH_TOPOLOGY();
	std::vector<BUTTON*> buttonTopology;
	std::vector<PEDAL*> pedals;
	
};

