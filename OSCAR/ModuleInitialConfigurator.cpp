#include "stdafx.h"
#include "ModuleInitialConfigurator.hpp"


ModuleInitialConfigurator::ModuleInitialConfigurator(MODULE* mod, boost::log::sources::logger_mt logger) :
	module_(mod),
	logger_(logger)
{
	BOOST_LOG(logger) << "INF " << "ModuleInitialConfigurator ctor";
	//createBinaryMask();
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
	std::vector<CONNECTOR*> conns;
	for (auto &conn : module_->children)
	{
		if (conn->name == "CONNECTOR")
		{
			conns.push_back(static_cast<CONNECTOR*>(conn));
		}
	}
	BOOST_LOG(logger_) << "INF " << "ModuleInitialConfigurator::setupConnectors: conns size is: " << conns.size();
	if (conns.size() < 8)
	{
		BOOST_LOG(logger_) << "INF " << "ModuleInitialConfigurator::setupConnectors: mask is greater than 255 ";
		std::bitset<8> bMask1(std::stoi(module_->mask));
		for (int i = 0; i < conns.size(); i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask1[i];
			conns[i]->value = bMask1[i];
		}
	}
	else if (conns.size() > 8 && conns.size() < 16)
	{
		std::bitset<8> bMask1(std::stoi(module_->mask));
		std::bitset<8> bMask2(std::stoi(module_->mask) - 255);
		BOOST_LOG(logger_) << "INF " << "ModuleInitialConfigurator::setupConnectors: mask is less than 255 ";
		for (int i = 0; i < 8; i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask1[i];
			conns[i]->value = bMask1[i];
		}
		for (int i = 8; i < conns.size(); i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask2[i - 8];
			conns[i]->value = bMask2[i - 8];
		}
	}
	else if (conns.size() > 16 && conns.size() < 24)
	{
		std::bitset<8> bMask1(std::stoi(module_->mask));
		std::bitset<8> bMask2(std::stoi(module_->mask) - 255);
		std::bitset<8> bMask3(std::stoi(module_->mask) - 511);
		BOOST_LOG(logger_) << "INF " << "ModuleInitialConfigurator::setupConnectors: mask is less than 255 ";
		for (int i = 0; i < 8; i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask1[i];
			conns[i]->value = bMask1[i];
		}
		for (int i = 8; i < 16; i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask2[i - 8];
			conns[i]->value = bMask2[i - 8];
		}
		for (int i = 16; i < conns.size(); i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask3[i - 16];
			conns[i]->value = bMask3[i - 16];
		}
	}
	else if (conns.size() > 24)
	{
		std::bitset<8> bMask1(std::stoi(module_->mask));
		std::bitset<8> bMask2(std::stoi(module_->mask) - 255);
		std::bitset<8> bMask3(std::stoi(module_->mask) - 511);
		std::bitset<8> bMask4(std::stoi(module_->mask) - 766);
		BOOST_LOG(logger_) << "INF " << "ModuleInitialConfigurator::setupConnectors: mask is less than 255 ";
		for (int i = 0; i < 8; i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask1[i];
			conns[i]->value = bMask1[i];
		}
		for (int i = 8; i < 16; i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask2[i - 8];
			conns[i]->value = bMask2[i - 8];
		}
		for (int i = 16; i < 24; i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask3[i - 16];
			conns[i]->value = bMask3[i - 16];
		}
		for (int i = 24; i < conns.size(); i++)
		{
			BOOST_LOG(logger_) << "DBG " << "ModuleInitialConfigurator::setupConnectors: conns->ID " << conns[i]->id << " bMask[" << i << "] " << bMask4[i - 24];
			conns[i]->value = bMask4[i - 24];
		}
	}
	
}