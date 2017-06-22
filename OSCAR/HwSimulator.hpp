#pragma once

#include "CAN.h"
#include "Objects\Obj.hpp"
#include "MODULE.hpp"
#include "HWSIMMODULE.hpp"
#include <vector>

class HwSimulator
{
public:
	HwSimulator();
	~HwSimulator();
	void simulateModules(std::vector<Obj*> modules);
	std::vector<HWSIMMODULE*> modules_;
	CAN* canPtr_;
	void sendMessage(CAN::messageCAN msg);
};

