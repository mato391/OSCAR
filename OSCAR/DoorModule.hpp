#pragma once
#include "Component.hpp"
#include <iostream>
#include <boost\algorithm\string.hpp>
#include <map>
#include "Objects\CP.hpp"
#include "Objects\DOOR.hpp"
#include "DOORS.hpp"
#include "ALARM.hpp"
#include "MODULE.hpp"
#include "EQM.hpp"
#include "CONNECTOR.hpp"
#include "RESULT.hpp"
#include "MODULE_TASK.hpp"
#include "CHANGE_CONNECTOR_STATE_TASK.h"
#include "Cache.hpp"

#include "commonGNDChecker.hpp"
#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <boost\optional.hpp>
#include "swconfig.hpp"

class DoorModule 
{
public:
	DoorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr);
	~DoorModule();
	void unlockDoors();
	void lockDoors();
	void initialize();
	void setup();
	std::vector<MODULE_TASK*>* tasks;
	boost::optional<std::string> changeConnectorState(int connectorId, int value);
	int getModuleProtocol() { return static_cast<int>(bdmModuleObj_->protocol); }
	void checkAndExecuteTask(Obj* obj);
private:
	boost::log::sources::logger_mt logger_;
	MODULE* bdmModuleObj_;
	std::vector<Obj*>* cache_;
	Cache* cachePtr_;
	DOORS* doorsObj_;
	int moduleTaskSubscrId_;
	void getBDMModules();
	void prepareTopology();
	void setDoorLockingInitStatus(DOOR::ELockingState lockState, std::string label);
	void setDoorOpeningInitStatus(DOOR::EOpeningState openState, std::string label);
	
	void displayTopology();
	std::string* checkIfDoorsAreClosed();
	bool checkIfBateryAlarmRaised();
	void createDoors(std::vector<CONNECTOR*> connectors);
	bool checkDoesDoorExist(std::string label);
	void changeDOORSOpeningStateIfNeeded(int value);
	void runTask(MODULE_TASK* task);
	void onOpen();
	void onClose();
	void setTimerForClose();
};

