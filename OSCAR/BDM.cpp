#include "stdafx.h"
#include "BDM.hpp"


BDM::BDM(std::string domain, boost::log::sources::logger_mt logger) 
{
	configuringState = EConfiguringState::online;
	logger_ = logger;
	BOOST_LOG(logger_) << "DEBUG " << "BDM ctor";
	this->domain = domain;
	name = "BDM";
}

BDM::~BDM()
{
}

RESULT* BDM::setup(int domain)
{
	BOOST_LOG(logger_) << "INF " << "BDM::setup: " << domain;
	std::string sdomain = (domain < 10 ) ? "0x0"+std::to_string(domain) : "0x" + std::to_string(domain);
	if (sdomain == "0x05")
	{
		doorModule_->setup();
		RESULT* result = new RESULT();
		result->applicant = "BDM";
		result->feedback = std::to_string(doorModule_->getModuleProtocol());
		result->status = RESULT::EStatus::success;
		return result;

	}
	else if (sdomain == "0x06")
	{
		lightModule_->setup();
		RESULT* result = new RESULT();
		result->applicant = "BDM";
		result->feedback = std::to_string(doorModule_->getModuleProtocol());
		result->status = RESULT::EStatus::success;
		return result;
	}
	return nullptr;
	//getResultAndSendToRouter();
}

CMESSAGE::CMessage* BDM::execute(CMESSAGE::CMessage* msg)
{
	BOOST_LOG(logger_) << "INF " << "BDM::execute " << msg;
	RESULT* res;
	CMESSAGE::CMessage* messg = nullptr;
	std::string domain =  msg->fromDomain;
	getBDMObjectIfNeeded();
	if (msg->getProtocol() == CMESSAGE::CMessage::EProtocol::CInitialProtocol
		&& msg->header == AA)
	{
		BOOST_LOG(logger_) << "INF " << "BDM::execute: " 
			<< "initial message detected from domain " 
			<< domain
			<< " bdmModules size: "
			<< bdmModules_.size();
		std::string moduleLabel;
		for (auto &module : bdmModules_)
		{
			BOOST_LOG(logger_) << "DBG " << "BDM::execute: " << "Module " << module.second->domain << " has been found";
			if (module.second->domain == domain)
			{
				module.second->detectionStatus = MODULE::EDetectionStatus::online;
				moduleLabel = module.first;
				BOOST_LOG(logger_) << "DBG " << "BDM::execute: " << "Module " << moduleLabel << " has been set";
				break;
			}
		}
		CMESSAGE::CInitialMessage* mesg = new CMESSAGE::CInitialMessage();
		mesg->header = AA;
		mesg->protocol = CMESSAGE::CMessage::EProtocol::CInitialProtocol;
		mesg->fromDomain = OWNID;
		mesg->toDomain = domain.substr(2, 2);
		mesg->optional1 = 0;
		mesg->optional2 = 0;
		return mesg;
	}
	else if (msg->getProtocol() == CMESSAGE::CMessage::EProtocol::CSimpleProtocol)
	{
		BOOST_LOG(logger_) << "INF " << "CSimpleProtocol";
	}
	return nullptr;
}

CMESSAGE::CMessage* BDM::convertResultToCMessage(RESULT* res)
{
	if (res->applicant == "DOOR_MODULE")
	{
		CMESSAGE::CInitialMessage* msg = new CMESSAGE::CInitialMessage();
		std::string tmpdomain = getDomainFor("BDM_DOOR");
		msg->toDomain = (tmpdomain.find("0x0") != std::string::npos) ? tmpdomain.substr(3, 1) : tmpdomain;
		BOOST_LOG(logger_) << "INF " << "BDM::convertResultToCMessage: msg->toDomain: " << msg->toDomain;
		
		msg->fromDomain = OWNID;
		if (res->feedback == "Initialize")
		{
			msg->header = AA;
			msg->optional1 = 0;
			msg->optional2 = 0;
			msg->protocol = CMESSAGE::CMessage::EProtocol::CInitialProtocol;
			return msg;
		}
	}
	
}

std::string BDM::getDomainFor(std::string label)
{
	for (const auto &mod : bdmModules_)
	{
		if (mod.second->label == label)
		{
			return mod.second->domain;
		}
	}
	return "";
}

void BDM::execute(std::string message)
{
	//0x01012
	std::string domain;
	std::string data;
	std::string port;
	std::string operation;
	getBDMObjectIfNeeded();
	BOOST_LOG(logger_) << "INFO " << "BDM::execute";
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
	//TODO: if domain is not 1 number
	//		port getting
	 
	
	std::string moduleLabel;
	if (operation == "aa")
	{
		BOOST_LOG(logger_) << "INF " << "BDM::execute: " << "Module " << domain << " has been detected";
		for (auto &module : bdmModules_)
		{
			BOOST_LOG(logger_) << "DBG " << "BDM::execute: " << "Module " << module.second->domain << " has been found";
			if (module.second->domain == domain)
			{
				module.second->detectionStatus = MODULE::EDetectionStatus::online;
				moduleLabel = module.first;
				BOOST_LOG(logger_) << "DBG " << "BDM::execute: " << "Module " << moduleLabel << " has been set";
				break;
				
			}
		}
		if (moduleLabel == "BDM_DOOR")
		{
			if (doorModule_ == nullptr)
				doorModule_ = new DoorModule(cache_, logger_);
			doorModule_->initialize();
		}
		else if (moduleLabel == "BDM_LIGHT")
		{
			if (lightModule_ == nullptr)
				lightModule_ = new LightModule(cache_, logger_);
			lightModule_->initialize();
		}
	}
	else
	{
		BOOST_LOG(logger_) << "INF " << "BDM::execute: " << "Module " << domain << " indicates";
		for (const auto &mod : bdmModules_)
		{
			if (mod.second->domain == domain)
			{
				moduleLabel = mod.first;
			}
		}
		if (moduleLabel == "BDM_DOOR")
		{
			doorModule_->changeConnectorState(port, operation);
		}
		else if (moduleLabel == "BDM_LIGHT")
		{
			lightModule_->changeConnectorStateIndication(port, operation);
		}
	}
}

void BDM::setConnector(std::string connId, std::string value)
{
	doorModule_->changeConnectorState(connId, value);
}

void BDM::execute(INTER_MODULE_OPERATION* imo)
{
	if (imo->operation == "DOOR_LOCKING_OPERATION")
	{
		if (imo->details == "00")
		{
			doorModule_->unlockDoors();
			lightModule_->blink(1);
			getResultAndSendToRouter("BDM_DOOR");
			getResultAndSendToRouter("BDM_LIGHT");
		}
		else if (imo->details == "01")
		{
			doorModule_->lockDoors();
			lightModule_->blink(2);
			getResultAndSendToRouter("BDM_DOOR");
			getResultAndSendToRouter("BDM_LIGHT");
		}
	}
	if (imo->operation == "GET_MIRROR_POS")
	{
		mirrorModule_->getMirrorPosition(std::stoi(imo->details));
		//IMO_RESULT should be created 
		
		
	}
}

void BDM::unlockDoors()
{
	doorModule_->unlockDoors();
	getResultAndSendToRouter("BDM_DOOR");
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
	setConfiguringStateIfNeeded();
	//mirrorModule_ = new MirrorModule(cache_, logger_);
	//mirrorModule_->initialize();
}

void BDM::getBDMObjectIfNeeded()
{
	if (bdmModules_.empty() || bdmModules_.size() < 2)
	{
		for (const auto &obj : *cache_)
		{
			if (obj->name == "EQM")
			{
				auto eqmObj = static_cast<EQM*>(obj);
				for (const auto &mod : eqmObj->modules_)
				{
					auto module = static_cast<MODULE*>(mod);
					if (module->label.find("BDM") != std::string::npos)
					{
						BOOST_LOG(logger_) << "INF " << "BDM::getBDMObjectIfNeeded: setting module " << module->label;
						bdmModules_[module->label] = module;
					}
				}
			}
		}
	}
}

void BDM::setConfiguringStateIfNeeded()
{
	if (/*lightModule_ != nullptr &&*/ doorModule_ != nullptr /* && mirrorModule_ != nullptr*/)	//comments should be deleted
	{
		BOOST_LOG(logger_) << "INF " << "BDM::setConfiguringStateIfNeeded: " << "OK";
		configuringState = EConfiguringState::configured;
	}
}

void BDM::blinkersRun(int times, int interval)
{
	for (int i = 0; i < times; i++)
	{
		//lightModule_->blink();
		boost::this_thread::sleep(boost::posix_time::millisec(interval));
	}
	
}

void BDM::getResultAndSendToRouter(std::string moduleLabel)
{
	RESULT* result = new RESULT();
	std::string domain;
	std::vector<Obj*>::iterator iter;
	for (const auto &mod : bdmModules_)
	{
		if (mod.second->label == moduleLabel)
		{
			domain = mod.second->domain;
			for (iter = mod.second->children.begin(); iter != mod.second->children.end(); iter++)
			{
				if ((*iter)->name == "RESULT")
				{
					result = static_cast<RESULT*>(*iter);
					if (result->applicant == moduleLabel)
						break;
				}
					
			}
		}
	}
	if (!result->feedback.empty())
	{
		std::string msg = domain + result->feedback;
		send(msg);
	}
	else
		BOOST_LOG(logger_) << "ERR " << "RESULT object does not exist";
	
}

