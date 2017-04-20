#include "stdafx.h"
#include "BDM.hpp"


BDM::BDM(std::string domain, boost::log::sources::logger_mt logger) 
{
	logger_ = logger;
	BOOST_LOG(logger_) << "DEBUG " << "BDM ctor";
	this->domain = domain;
	name = "BDM";
	
}

BDM::~BDM()
{
}

void BDM::execute(std::string message)
{
	//0x01012
	BOOST_LOG(logger_) << "INFO " << "BDM::execute";
	std::string data = message.substr(4, 3);
	std::string port = data.substr(1, 1);
	std::string operation = data.substr(2, 1);
	if (std::stoi(port) <= 5)
	{
		if (operation == "0")
			doorModule_->changeOpeningState(port, DOOR::EOpeningState::closed);
		else if (operation == "1")
			doorModule_->changeOpeningState(port, DOOR::EOpeningState::opened);
	}
	else
	{

	}
}

void BDM::execute(INTER_MODULE_OPERATION* imo)
{
	if (imo->operation == "DOOR_LOCKING_OPERATION")
	{
		if (imo->details == "00")
			doorModule_->unlockDoors();
		else if (imo->details == "01")
			doorModule_->lockDoors();
	}
}

void BDM::unlockDoors()
{
	doorModule_->unlockDoors();
}

void BDM::lockDoors()
{
	doorModule_->lockDoors();
}

void BDM::initialize()
{
	doorModule_ = new DoorModule(cache_, logger_);
	doorModule_->initialize();
	lightModule_ = new LightModule(cache_, logger_);
	lightModule_->initialize();
}

void BDM::blinkersRun(int times, int interval)
{
	for (int i = 0; i < times; i++)
	{
		lightModule_->blink();
		boost::this_thread::sleep(boost::posix_time::millisec(interval));
	}
	
}


