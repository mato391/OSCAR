#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Component.hpp"
#include "DoorModule.hpp"
#include "LightModule.h"
#include "MirrorModule.hpp"
#include "Objects\Obj.hpp"
#include <boost\thread.hpp>
#include "RESULT.hpp"
#include "MODULE.hpp"
#include "CMessage.hpp"
#include "TaskCreator.hpp"
#include "LIGHT_WELCOMING_TASK.h"
#include "LIGHT_GOODBYE_TASK.h"



#define OWNID 100
#define DD 221
#define EE 238
#define CC 204


class BDM : public Component
{
public:
	BDM(std::string domain, boost::log::sources::logger_mt logger);
	~BDM();
	void execute(std::string message);	//TO BE DELETED WHEN ALL COMPONENTS WILL USING CMESSAGE
	void execute(INTER_MODULE_OPERATION* imo);
	CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg);
	void initialize();
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(CMESSAGE::CMessage*)> func) { send = func; }
	RESULT* setup(int domain);
	void setup(std::string domain) {};//TO BE DELETED WHEN ALL COMPONENTS WILL USING RESULTS
	int runtime() {};
	void unlockDoors();
	void lockDoors();
	void setConfiguringStateIfNeeded();
	void getSubscription(Obj* obj);
private:
	DoorModule* doorModule_;
	LightModule* lightModule_;
	MirrorModule* mirrorModule_;
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	std::map<std::string, MODULE*> bdmModules_;
	TaskCreator* taskCreator_;

	void getBDMObjectIfNeeded();
	CMESSAGE::CMessage* convertResultToCMessage(RESULT* res);
	std::string getDomainFor(std::string label);
	MODULE* getModuleWithDomain(std::string domain);
	void doTasks();
};

