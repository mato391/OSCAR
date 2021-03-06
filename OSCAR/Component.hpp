#pragma once
#include <string>
#include <vector>
#include "Objects\Obj.hpp"
#include "INTER_MODULE_OPERATION.h"
#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include "RESULT.hpp"
#include "CMessage.hpp"
#include "Cache.hpp"
#define AA 170

class Component
{
public:
	std::string domain;
	std::string name;
	Cache* cachePtr;
	enum class EConfiguringState
	{
		online,
		initialized,
		configured
	};
	EConfiguringState configuringState;
	std::function<void(CMESSAGE::CMessage*)> send;
	virtual void setSenderPtr(std::function<void(CMESSAGE::CMessage*)> func) = 0;
	virtual void setCache(std::vector<Obj*>* cache) = 0;
	void setCache(Cache* cache) { cachePtr = cache; };
	virtual void setComponentsCache(std::vector<Component*>* componentCache) = 0;
	virtual void execute(std::string message) = 0;
	virtual CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg) = 0;
	virtual void execute(INTER_MODULE_OPERATION* imo) = 0;
	virtual void initialize() = 0;
	virtual void setup(std::string domain) = 0;
	virtual RESULT* setup(int domain) = 0;
	//virtual int runtime() = 0;
	boost::log::sources::logger_mt logger_;
	
};

