#include "stdafx.h"
#include "ModuleInitialConfigurator.hpp"


ModuleInitialConfigurator::ModuleInitialConfigurator(MODULE* mod, boost::log::sources::logger_mt logger) :
	module_(mod),
	logger_(logger)
{
	BOOST_LOG(logger) << "INF " << "ModuleInitialConfigurator ctor";
	createBinaryMask();
	setupConnectors();
}


ModuleInitialConfigurator::~ModuleInitialConfigurator()
{
}

void ModuleInitialConfigurator::createBinaryMask()
{
	BOOST_LOG(logger_) << "INF " << "ModuleInitialConfigurator::createBinaryMask: for module: " << module_->domain << " dec: "
		<< module_->mask<< std::endl;
	binaryMask_ = std::bitset<16>(std::stoi(module_->mask));
}

int ModuleInitialConfigurator::convertToDec(std::string sign)
{
	if (sign == "0")
	{
		return 0;
	}
	else if (sign == "1")
	{
		return 1;
	}
	else if (sign == "2")
	{
		return 2;
	}
	else if (sign == "3")
	{
		return 3;
	}
	else if (sign == "4")
	{
		return 4;
	}
	else if (sign == "5")
	{
		return 5;
	}
	else if (sign == "6")
	{
		return 6;
	}
	else if (sign == "7")
	{
		return 7;
	}
	else if (sign == "8")
	{
		return 8;
	}
	else if (sign == "9")
	{
		return 9;
	}
	else if (sign == "A" || sign == "a")
	{
		return 10;
	}
	else if (sign == "B" || sign == "b")
	{
		return 11;
	}
	else if (sign == "C" || sign == "c")
	{
		return 12;
	}
	else if (sign == "D" || sign == "d")
	{
		return 13;
	}
	else if (sign == "E" || sign == "e")
	{
		return 14;
	}
	else if (sign == "F" || sign == "f")
	{
		return 15;
	}
}

void ModuleInitialConfigurator::setupConnectors()
{
	BOOST_LOG(logger_) << "INF " << "ModuleInitialConfigurator::setupConnectors: Mask size is: " << module_->connectors_.size();
	for (auto &connGr : module_->connectors_)
	{
		int i = 0;
		for (auto &conn : connGr)
		{
			auto connector = static_cast<CONNECTOR*>(conn);
			connector->value = binaryMask_[i];	//crash here
			BOOST_LOG(logger_) << "INF "
				<< "ModuleInitialConfigurator::setupConnectors: CONNECTOR ID: "
				<< i
				<< " Connector value: "
				<< connector->value;
			i++;
		}
		
	}
}