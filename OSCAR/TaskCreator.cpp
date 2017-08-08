#include "stdafx.h"
#include "TaskCreator.hpp"


TaskCreator::TaskCreator(std::map<std::string, MODULE*> * bdmModules,
	boost::log::sources::logger_mt logger, Cache* cache) 
	: bdmModules_(bdmModules),
	cache_(cache)
{
	logger_ = logger;
	BOOST_LOG(logger_) << "TaskCreator::ctor";
}


TaskCreator::~TaskCreator()
{
}

void TaskCreator::convertAndPushTask(CMESSAGE::CMessage* msg)
{
	BOOST_LOG(logger_) << "INF " << "TaskCreator::convertAndPushTask: " << msg->fromDomain;
	MODULE_TASK* modTask = new MODULE_TASK();
	if (msg->protocol == CMESSAGE::CMessage::EProtocol::CSimpleProtocol)
	{
		CMESSAGE::CSimpleMessage * sMsg = static_cast<CMESSAGE::CSimpleMessage*>(msg);
		modTask = new CHANGE_CONNECTOR_STATE_TASK(sMsg->port, sMsg->value);
		modTask->taskFor = msg->fromDomain;
		BOOST_LOG(logger_) << "INF " << "TaskCreator::convertAndPushTask : creating CHANGE_CONNECTOR_STATE_TASK with values " << sMsg->port << ", " << sMsg->value;
	}
	else if (msg->protocol == CMESSAGE::CMessage::EProtocol::CExtendedProtocol)
	{
		CMESSAGE::CExtendedMessage * sMsg = static_cast<CMESSAGE::CExtendedMessage*>(msg);
		modTask = new CHANGE_CONNECTOR_STATE_TASK(sMsg->port, sMsg->value);
		modTask->taskFor = msg->fromDomain;
		BOOST_LOG(logger_) << "INF " << "TaskCreator::convertAndPushTask : creating CHANGE_CONNECTOR_STATE_TASK with values " << sMsg->port << ", " << sMsg->value;
	}
	else if (msg->protocol == CMESSAGE::CMessage::EProtocol::CMaskProtocol 
		|| msg->protocol == CMESSAGE::CMessage::EProtocol::CMaskExtendedProtocol)
	{
		CMESSAGE::CMaskMessage * sMsg = static_cast<CMESSAGE::CMaskMessage*>(msg);
		modTask = new MASK_CONNECTORS_STATE();
		modTask->taskFor = msg->fromDomain;
		static_cast<MASK_CONNECTORS_STATE*>(modTask)->mask1 = sMsg->mask1;
		static_cast<MASK_CONNECTORS_STATE*>(modTask)->mask2 = sMsg->mask2;
		BOOST_LOG(logger_) << "INF " << "TaskCreator::convertAndPushTask : creating MASK_CONNECTOR_STATE with values " << sMsg->mask1 << ", " << sMsg->mask2;
	}
	for (auto &module : *bdmModules_)
	{
		if (module.second->domain == msg->fromDomain)
		{
			cache_->addToChildren( module.second, modTask);
			//BOOST_LOG(logger_) << "INF " << "TaskCreator::convertAndPushTask : Adding MODULE_TASK to tasks for module: " << module.second->domain;
			break;
		}
	}
}