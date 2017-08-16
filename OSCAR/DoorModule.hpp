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
#include "CHANGE_CONNECTOR_DONE_IND.hpp"
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
	int getModuleProtocol() { return static_cast<int>(bdmModuleObj_->protocol); }
private:
	boost::log::sources::logger_mt logger_;
	MODULE* bdmModuleObj_;
	Cache* cachePtr_;
	DOORS* doorsObj_;
	int moduleTaskSubscrId_;
	int ccIndSubscrId_;

	void setDoorsInitialStates();
	CONNECTOR* getGNDConnector(std::vector<int> refs);
	CONNECTOR* getNotGNDConnector(std::vector<int> refs);
	void getBDMModules();
	void prepareTopology();
	DOOR* getDoorByLabel(std::string label);
	DOOR* getDoorByRefId(int ref);

	void changeConnectorIndHandler(Obj* obj);

	
};

