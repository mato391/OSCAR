#include "stdafx.h"
#include "WCM.hpp"


WCM::WCM(std::string domain, boost::log::sources::logger_mt logger)
{
	logger_ = logger;
	name = "WCM";
	BOOST_LOG(logger) << "DEBUG " << "WCM ctor ";
	this->domain = domain;
	userManager_ = new UserManager();
	
}


WCM::~WCM()
{
}

void WCM::ethernetInitialize()
{
	ethIntConfigurator_->getEthernetInformation();
}

void WCM::initialize(std::string subcomponent)
{
	prepareTopology();
	getBDM();
	cardPortObj_ = new CARD_PORT();
}

void WCM::prepareTopology()
{

}

void WCM::getBDM()
{
	for (auto &component : *componentCache_)
	{
		if (component->name == "BDM")
		{
			BOOST_LOG(logger_) << "INFO " << "WCM::getBDM found";
			bdmObjPtr_ = static_cast<BDM*>(component);
			return;
		}
	}
}

void WCM::execute(std::string message)
{
	//0x0099992001
	if (bdmObjPtr_ == nullptr)
		getBDM();
	if (eqmObjPtr_ == nullptr)
		getEQM();
	BOOST_LOG(logger_) << "INFO " << "WCM::execute: " << message;
	std::string serialNumber = message.substr(4, 4);
	std::string power = message.substr(8, 2);
	std::string operation = message.substr(10, 2);
	if (serialNumber.substr(0, 1) == "9")
	{
		rcObj_ = new REMOTE_CONTROLLER(serialNumber);
		if (!rcObj_->login())
		{
			delete rcObj_;
			return;
		}
	}
	
		
	if (std::stoi(power) <= 20)
	{
		BOOST_LOG(logger_) << "INFO " << "WCM::execute: BATTERY_LOW";
		cache_->push_back(new ALARM("Battery low", 1001, "WCM"));
		bdmObjPtr_->execute(new INTER_MODULE_OPERATION("DOOR_LOCKING_OPERATION", operation));
		
	}
	else
	{
		bdmObjPtr_->execute(new INTER_MODULE_OPERATION("DOOR_LOCKING_OPERATION", operation));
		
	}
	userManager_->setupUser(serialNumber);
	auto userObj = userManager_->getUser();
	cache_->push_back(userObj);
	BOOST_LOG(logger_) << "DBG " << userObj->username << " sn: " << userObj->rcSerialCode << " isNew " << userObj->isNew;
	if (userObj->isNew)
	{
		std::cout << "SHOULD BE HERE" << std::endl;
		executeOnUIA(new INTER_MODULE_OPERATION("NEW_USER", userObj->rcSerialCode));
		return;
	}
	if (message.substr(message.size() - 1, 1) == "2")
	{
		BOOST_LOG(logger_) << "INFO " << "WCM::execute: CARD detected";
		if (serialNumber == rcObj_->serialNumber)
		{
			BOOST_LOG(logger_) << "INFO " << "WCM::execute: CARD validated";
			cardPortObj_->detectionState = CARD_PORT::EDetectionState::detectedDone;
			cardPortObj_->cardRef = rcObj_->serialNumber;
		}
		else
		{
			cardPortObj_->detectionState = CARD_PORT::EDetectionState::notValidated;
		}
		return;
	}
	if (message.substr(message.size() - 1, 1) == "3")
	{
		BOOST_LOG(logger_) << "INFO " << "WCM::execute: CARD pulled";
		cardPortObj_->detectionState = CARD_PORT::EDetectionState::notDetected;
		return;
	}
}

void WCM::execute(INTER_MODULE_OPERATION* imo)
{
	if (eqmObjPtr_ == nullptr)
		getEQM();
	if (imo->operation == "ETHERNET_INIT")
	{
		wcmModule_ = new MODULE();
		wcmModule_->domain = domain;
		wcmModule_->label = name;
		eqmObjPtr_->addModule(wcmModule_);
		ethIntConfigurator_ = new EthernetIntrfaceConfigurator(cache_, logger_);
		ethernetInitialize();
		displayTopology();
	}
	startWebUIServiceIsNeeded();
}

void WCM::startWebUIServiceIsNeeded()
{
	for (const auto &connVec : wcmModule_->connectors_)
	{
		for (const auto &conn : connVec)
		{
			auto netIf = static_cast<NET_INTERFACE*>(conn);
			if (netIf->ifName.find("Wireless") != std::string::npos)
			{
				//WebUiService Here should be started
				BOOST_LOG(logger_) << "INF " << "WCM::startWebUIServiceIsNeeded: starting web service";
				return;
			}
		}
	}
	BOOST_LOG(logger_) << "INF " << "WCM::startWebUIServiceIsNeeded: Wireless connection does not exist";
}

void WCM::executeOnUIA(INTER_MODULE_OPERATION* imo)
{
	for (auto &component : *componentCache_)
	{
		BOOST_LOG(logger_) << "DBG " << "WCM::executeOnUIA " << component->name;
		
		if (component->name == "UIA")
		{
			component->execute(imo);
			return;
		}
	}
}

void WCM::getEQM()
{
	for (auto &obj : *cache_)
	{
		if (obj->name == "EQM")
		{
			eqmObjPtr_ = static_cast<EQM*>(obj);
		}
	}
}

void WCM::displayTopology()
{
	for (const auto &connsVec : wcmModule_->connectors_)
	{
		BOOST_LOG(logger_) << "DBG " << "CONNECTOR: ";
		for (const auto &conn : connsVec)
		{
			auto netIf = static_cast<NET_INTERFACE*>(conn);
			BOOST_LOG(logger_) << "DBG " << "NAME: " << netIf->ifName;
			BOOST_LOG(logger_) << "DBG " << "IP: " << netIf->ip;
			BOOST_LOG(logger_) << "DBG " << "MASK: " << netIf->mask;
			BOOST_LOG(logger_) << "DBG " << "GW: " << netIf->gateway;
			BOOST_LOG(logger_) << "DBG " << "DHCP: " << netIf->dhcp;
		}
		
	}
}


