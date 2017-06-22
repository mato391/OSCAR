#include "stdafx.h"
#include "ModuleInitialConfigurator.hpp"


ModuleInitialConfigurator::ModuleInitialConfigurator(MODULE* mod) :
	module_(mod)
{
	createBinaryMask();
	setupConnectors();
}


ModuleInitialConfigurator::~ModuleInitialConfigurator()
{
}

void ModuleInitialConfigurator::createBinaryMask()
{
	std::cout << "ModuleInitialConfigurator::createBinaryMask: " << std::endl;
	int maskSize = module_->mask.size();
	for (int i = 0; i < maskSize; i++)
	{
		convertToBinary(module_->mask.substr(i, 1));
	}
	for (const auto &n : binaryMask_)
	{
		std::cout << n;
	}
}

void ModuleInitialConfigurator::convertToBinary(std::string sign)
{
	if (sign == "0")
	{
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
	}
	else if (sign == "1")
	{
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
	}
	else if (sign == "2")
	{
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
	}
	else if (sign == "3")
	{
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
	}
	else if (sign == "4")
	{
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
	}
	else if (sign == "5")
	{
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
	}
	else if (sign == "6")
	{
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
	}
	else if (sign == "7")
	{
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
	}
	else if (sign == "8")
	{
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
	}
	else if (sign == "9")
	{
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
	}
	else if (sign == "A" || sign == "a")
	{
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
	}
	else if (sign == "B" || sign == "b")
	{
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
	}
	else if (sign == "C" || sign == "c")
	{
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
		binaryMask_.push_back(0);
	}
	else if (sign == "D" || sign == "d")
	{
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
		binaryMask_.push_back(1);
	}
	else if (sign == "E" || sign == "e")
	{
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
		binaryMask_.push_back(0);
	}
	else if (sign == "F" || sign == "f")
	{
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
		binaryMask_.push_back(1);
	}
}

void ModuleInitialConfigurator::setupConnectors()
{
	for (auto &connGr : module_->connectors_)
	{
		for (auto &conn : connGr)
		{
			auto connector = static_cast<CONNECTOR*>(conn);
			connector->value = binaryMask_[connector->id];	//crash here
		}
		for (const auto &conn : connGr)
		{
			auto connector = static_cast<CONNECTOR*>(conn);
			std::cout << "CONNECTOR VALUE: " << connector->value << std::endl;
		}
	}
}