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

void BDM::execute(std::string message)
{
	//0x01012
	getBDMObjectIfNeeded();
	BOOST_LOG(logger_) << "INFO " << "BDM::execute";
	std::string domain = message.substr(0, 4);
	std::string data = message.substr(4, 3);
	std::string port = data.substr(0, 2);
	std::string operation = data.substr(2, 1);
	std::string moduleLabel;
	if (data.find("AA") != std::string::npos)
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

