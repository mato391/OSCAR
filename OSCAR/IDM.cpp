#include "stdafx.h"
#include "IDM.hpp"


IDM::IDM(std::string domain, boost::log::sources::logger_mt logger)
{
	configuringState = EConfiguringState::online;
	logger_ = logger;
	BOOST_LOG(logger_) << "DEBUG " << "IDM ctor";
	this->domain = domain;
	name = "IDM";
}


IDM::~IDM()
{
}
void IDM::execute(std::string message)
{
}
void IDM::execute(INTER_MODULE_OPERATION* imo)
{
	if (imo->operation == "USER_LAMP_ON")
	{
		BOOST_LOG(logger_) << "INF " << "IDM::execute " << imo->operation;
	}
}
CMESSAGE::CMessage* IDM::execute(CMESSAGE::CMessage* msg)
{
	CMESSAGE::CInitialMessage* mesg = new CMESSAGE::CInitialMessage();
	mesg->header = AA;
	mesg->protocol = CMESSAGE::CMessage::EProtocol::CInitialProtocol;
	mesg->fromDomain = OWNID;
	mesg->toDomain = domain.substr(2, 2);
	mesg->optional1 = 0;
	mesg->optional2 = 0;
	return mesg;
}
void IDM::initialize()
{
	lightingModulePtr_ = new LightingModule();
	//ButtonsAndSwitchesModule
	//ComfortModule
	configuringState = EConfiguringState::configured;
}

RESULT* IDM::setup(int domain)
{
	RESULT* result = new RESULT();
	result->applicant = "IDM";
	result->feedback = "1";
	result->status = RESULT::EStatus::success;
	return result;
}
