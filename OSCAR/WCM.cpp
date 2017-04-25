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

void WCM::initialize()
{
	getBDM();
	cardPortObj_ = new CARD_PORT();
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


