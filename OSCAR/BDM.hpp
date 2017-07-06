#pragma once
#include <vector>
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
#include <vector>

#define OWNID 100
#define DD 221
#define EE 238


class BDM : public Component
{
public:
	BDM(std::string domain, boost::log::sources::logger_mt logger);
	~BDM();
	void execute(std::string message);
	void execute(INTER_MODULE_OPERATION* imo);
	CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg);
	void initialize();
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(std::string)> func) { send = func; }
	RESULT* setup(int domain);
	void setup(std::string domain) {};
	void unlockDoors();
	void lockDoors();
	void setConfiguringStateIfNeeded();
	void openWindow(std::string port);
	void closeWindow(std::string port);
	void lockWindow();
	void unlockWindow();
private:
	DoorModule* doorModule_;
	LightModule* lightModule_;
	MirrorModule* mirrorModule_;
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	std::map<std::string, MODULE*> bdmModules_;
	void getBDMObjectIfNeeded();
	void setConnector(std::string connId, std::string value);
	void blinkersRun(int times, int interval);
	void getResultAndSendToRouter(std::string moduleLabel);
	CMESSAGE::CMessage* convertResultToCMessage(RESULT* res);
	std::string getDomainFor(std::string label);
	void action(boost::optional<std::string> impuls);
};

