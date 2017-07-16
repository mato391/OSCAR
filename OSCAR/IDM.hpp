#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Component.hpp"
#include "Objects\Obj.hpp"
#include <boost\thread.hpp>
#include "RESULT.hpp"
#include "MODULE.hpp"
#include "CMessage.hpp"
#include "LightingModule.hpp"

#define OWNID 100


class IDM :
	public Component
{
public:
	IDM(std::string domain, boost::log::sources::logger_mt logger);
	~IDM();
	void execute(std::string message);	//TO BE DELETED WHEN ALL COMPONENTS WILL USING CMESSAGE
	void execute(INTER_MODULE_OPERATION* imo);
	CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg);
	void initialize();
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(CMESSAGE::CMessage*)> func) { send = func; }
	RESULT* setup(int domain);
	void setup(std::string domain) {};
private:
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	LightingModule* lightingModulePtr_;
	
};

