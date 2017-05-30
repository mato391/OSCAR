#pragma once
#include <string>
#include <vector>
#include "Objects\Obj.hpp"
#include "INTER_MODULE_OPERATION.h"
#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class Component
{
public:
	std::string domain;
	std::string name;
	enum class EConfiguringState
	{
		online,
		initialized,
		configured
	};
	EConfiguringState configuringState;
	std::function<void(std::string)> send;
	virtual void setSenderPtr(std::function<void(std::string)> func) = 0;
	virtual void setCache(std::vector<Obj*>* cache) = 0;
	virtual void setComponentsCache(std::vector<Component*>* componentCache) = 0;
	virtual void execute(std::string message) = 0;
	virtual void execute(INTER_MODULE_OPERATION* imo) = 0;
	virtual void initialize(std::string subcomponent) = 0;
	
	boost::log::sources::logger_mt logger_;
	
};

