#include "stdafx.h"
#include "HwSimulator.hpp"


HwSimulator::HwSimulator()
{
	canPtr_ = new CAN(100);
}


HwSimulator::~HwSimulator()
{
}

void HwSimulator::simulateModules(std::vector<Obj*> modules)
{
	for (const auto &obj : modules)
	{
		auto module = static_cast<MODULE*>(obj);
		modules_.push_back(new HWSIMMODULE(module->label, std::stoi(module->domain)));
	}
}

void HwSimulator::sendMessage(CAN::messageCAN msg)
{
	for (auto module : modules_)
	{
		module->can->messageRx = msg;
		module->can->setMsgAvailable();
	}
}
