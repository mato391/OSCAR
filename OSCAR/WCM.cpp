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

void WCM::initialize()
{
	getWCM();
	ethIntConfigurator_ = new EthernetIntrfaceConfigurator(cache_, logger_);
	getAllAntenaDecives();
	prepareTopology();
	this->configuringState = EConfiguringState::configured;
}

void WCM::prepareTopology()
{
	for (const auto &antenna : antenaDevices_)
	{
		if (antenna->label.find( "RC" ) != std::string::npos && rcMan_ == nullptr)
		{
			rcMan_ = new rcManager(wcmModule_, logger_);
			rcMan_->prepareRCInterface();
		}
		else if (antenna->label == "CARD_PORT")
		{
			//TODO
		}
	}
	ethernetInitialize();
}

void WCM::getAllAntenaDecives()
{
	//BOOST_LOG(logger_) << "DBG " << "WCM::getAllAntenaDecives: " << wcmModule_->connectors_[0].size();
	for (const auto &objVec : wcmModule_->connectors_)
	{
		for (auto &obj : objVec)
		{
			BOOST_LOG(logger_) << "DBG " << "WCM::getAllAntenaDecives: checking: " << obj->name;
			if (obj->name == "ANTENNA")
			{
				antenaDevices_.push_back(static_cast<ANTENNA*>(obj));
			}
		}
	}
	BOOST_LOG(logger_) << "INF " << "WCM::getAllAntenaDecives: " << antenaDevices_.size() << " antena devices have been found";
}

void WCM::getWCM()
{
	for (auto &obj : *cache_)
	{
		if (obj->name == "EQM")
		{
			for (const auto &mod : static_cast<EQM*>(obj)->modules_)
			{
				auto module = static_cast<MODULE*>(mod);
				if (module->label == "WCM")
				{
					wcmModule_ = module;
					return;
				}
			}
		}
	}
}

void WCM::execute(std::string message)
{
	//0x0799792001
	std::string domain;
	std::string serialNumber;
	std::string power;
	std::string operation;
	std::string moduleLabel;
	int startPoint = std::string::npos;
	startPoint = message.find("aa");
	if (startPoint == std::string::npos)
	{
		startPoint = message.find("bb");
		if (startPoint == std::string::npos)
		{
			startPoint = message.find("cc");
		}
	}
	if (startPoint == 1)
	{
		domain = "0x0" + message.substr(0, 1);
		operation = message.substr(1, 2);

	}
	if (operation == "aa")
	{
		BOOST_LOG(logger_) << "INF " << "WCM::execute: " << "Module " << domain << " has been detected";
		initialize();
		wcmModule_->detectionStatus = MODULE::EDetectionStatus::online;
	}
	else
	{
			domain = message.substr(0, 4);
			serialNumber = message.substr(4, 4);
			power = message.substr(8, 2);
			operation = message.substr(10, 2);
		
	
		if (bdmObjPtr_ == nullptr)
			getBDM();
		if (eqmObjPtr_ == nullptr)
			getEQM();
		BOOST_LOG(logger_) << "INFO " << "WCM::execute: " << message;
	
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

void WCM::getBDM()
{
	for (const auto &component : *componentCache_)
	{
		if (component->name == "BDM")
		{
			bdmObjPtr_ = static_cast<BDM*>(component);
			return;
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


