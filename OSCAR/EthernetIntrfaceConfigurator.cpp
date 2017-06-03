#include "stdafx.h"
#include "EthernetIntrfaceConfigurator.hpp"


EthernetIntrfaceConfigurator::EthernetIntrfaceConfigurator(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger) : 
	cache_(cache),
	logger_(logger)
{
	BOOST_LOG(logger_) << "INF " << "EthernetInterfaceConfigurator ctor";
}


EthernetIntrfaceConfigurator::~EthernetIntrfaceConfigurator()
{
}

void EthernetIntrfaceConfigurator::getEthernetInformation()
{
	system("python D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Scripts\\Ethernet\\getEth.py");
	BOOST_LOG(logger_) << "DBG " << "EthernetIntrfaceConfigurator::getEthernetInformation: " << "HWAPI script done";
	std::fstream file("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Scripts\\Ethernet\\ETH_CONFIG.txt", std::ios::in);
	std::string content;
	file >> content;
	file.close();
	createNetworkInformations(content);
}

void EthernetIntrfaceConfigurator::createNetworkInformations(std::string data)
{
	getWCMModule();
	std::vector<std::string> sdata;
	boost::split(sdata, data, boost::is_any_of("#"));
	std::vector<Obj*> netIfs;
	for (const auto &inter : sdata)
	{
		std::vector<std::string> ifdata;
		boost::split(ifdata, inter, boost::is_any_of(";"));
		NET_INTERFACE* netIf = new NET_INTERFACE();
		
		for (const auto &element : ifdata)
		{
			std::vector<std::string> selement;
			boost::split(selement, element, boost::is_any_of(":"));
			if (selement[0] == "NAME")
			{
				netIf->ifName = selement[1];
			}
			else if (selement[0] == "IP")
			{
				netIf->ip = selement[1];
			}
			else if (selement[0] == "MASK")
			{
				netIf->mask = selement[1];
			}
			else if (selement[0] == "GW")
			{
				netIf->gateway = selement[1];
			}
			else if (selement[0] == "DHCP")
			{
				if (selement[1].find("No") != std::string::npos)
					netIf->dhcp = false;
				else
					netIf->dhcp = true;
			}
		}
		netIfs.push_back(netIf);
	}
	if (!netIfs.empty() && wcmModule_ != nullptr)
		wcmModule_->connectors_.push_back(netIfs);
	else if (wcmModule_ == nullptr)
		BOOST_LOG(logger_) << "ERR " << "WCM module does not exist";
	else
		BOOST_LOG(logger_) << "WRN " << "No Ethernet interface";
}

void EthernetIntrfaceConfigurator::getWCMModule()
{
	for (auto &obj : *cache_)
	{
		if (obj->name == "EQM")
		{
			auto eqmObjPtr = static_cast<EQM*>(obj);
			for (auto &module : eqmObjPtr->modules_)
			{
				auto mod = static_cast<MODULE*>(module);
				if (mod->label == "WCM")
				{
					wcmModule_ = mod;
					return;
				}
					
			}
		}
	}
}