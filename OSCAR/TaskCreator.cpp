#include "stdafx.h"
#include "TaskCreator.hpp"


TaskCreator::TaskCreator(std::map<std::string, MODULE*> * bdmModules,
	boost::log::sources::logger_mt logger) 
	: bdmModules_(bdmModules)
{
	logger_ = logger;
	BOOST_LOG(logger_) << "TaskCreator::ctor";
}


TaskCreator::~TaskCreator()
{
}

void TaskCreator::convertAndPushTask(CMESSAGE::CMessage* msg)
{
	BOOST_LOG(logger_) << "INF " << "TaskCreator::convertAndPushTask";
	MODULE_TASK* modTask;
	if (msg->protocol == CMESSAGE::CMessage::EProtocol::CSimpleProtocol)
	{
		CMESSAGE::CSimpleMessage * sMsg = static_cast<CMESSAGE::CSimpleMessage*>(msg);
		modTask = new CHANGE_CONNECTOR_STATE_TASK(sMsg->port, sMsg->value);
		BOOST_LOG(logger_) << "INF " << "TaskCreator::convertAndPushTask : creating CHANGE_CONNECTOR_STATE_TASK with values " << sMsg->port << ", " << sMsg->value;
	}
	for (auto &module : *bdmModules_)
	{
		if (module.second->domain == msg->fromDomain)
		{
			module.second->tasks.push_back(modTask);
			BOOST_LOG(logger_) << "INF " << "TaskCreator::convertAndPushTask : Adding MODULE_TASK to tasks for module: " << module.second->domain;
			break;
		}
	}
}