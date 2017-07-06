#pragma once
#include "EQM.hpp"
#include "MODULE.hpp"
#include "CONNECTOR.hpp"
#include <iostream>
#include <bitset>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class ModuleInitialConfigurator
{
public:
	ModuleInitialConfigurator(MODULE* mod, boost::log::sources::logger_mt logger);
	~ModuleInitialConfigurator();
private:
	MODULE* module_;
	std::bitset<16> binaryMask_;
	boost::log::sources::logger_mt logger_;

	void createBinaryMask();
	int convertToDec(std::string sign);
	void setupConnectors();
};

