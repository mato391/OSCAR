#pragma once
#include "EQM.hpp"
#include "MODULE.hpp"
#include "CONNECTOR.hpp"
#include <iostream>

class ModuleInitialConfigurator
{
public:
	ModuleInitialConfigurator(MODULE* mod);
	~ModuleInitialConfigurator();
private:
	MODULE* module_;
	std::vector<int> binaryMask_;

	void createBinaryMask();
	void convertToBinary(std::string sign);
	void setupConnectors();
};

