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


class BDM : public Component
{
public:
	BDM(std::string domain, boost::log::sources::logger_mt logger);
	~BDM();
	void execute(std::string message);
	void execute(INTER_MODULE_OPERATION* imo);
	void initialize(std::string component);
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(std::string)> func) { send = func; }
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
	void blinkersRun(int times, int interval);
	void getResultAndSendToRouter(std::string moduleLabel);
	
};

