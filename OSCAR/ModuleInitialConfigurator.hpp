#pragma once
#include "EQM.hpp"
#include "MODULE.hpp"
#include "CONNECTOR.hpp"
#include <iostream>
#include <bitset>

class ModuleInitialConfigurator
{
public:
	ModuleInitialConfigurator(MODULE* mod);
	~ModuleInitialConfigurator();
private:
	MODULE* module_;
	std::bitset<16> binaryMask_;

	void createBinaryMask();
	int convertToDec(std::string sign);
	void setupConnectors();
};

