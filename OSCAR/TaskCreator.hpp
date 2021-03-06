#pragma once
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

#include <map>
#include <string>
#include "MODULE.hpp"
#include "CMessage.hpp"
#include "MODULE_TASK.hpp"
#include "CHANGE_CONNECTOR_STATE_TASK.h"
#include "MASK_CONNECTORS_STATE.hpp"
#include "Cache.hpp"


class TaskCreator
{
public:
	TaskCreator(std::map<std::string, MODULE*>* bdmModules,
		boost::log::sources::logger_mt logger, Cache* cache);
	~TaskCreator();
	void convertAndPushTask(CMESSAGE::CMessage* msg);
private:
	std::map<std::string, MODULE*>* bdmModules_;
	boost::log::sources::logger_mt logger_;
	Cache* cache_;
};

