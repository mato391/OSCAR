#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <bitset>
#include "Objects\Obj.hpp"
#include "Objects\CP.hpp"
#include "Objects\LIGHT.hpp"
#include "DOORS.hpp"
#include "LIGHTES.hpp"
#include "MODULE.hpp"
#include "EQM.hpp"
#include "POWER_GROUP.hpp"
#include "RESULT.hpp"
#include "MODULE_TASK.hpp"
#include "CHANGE_CONNECTOR_STATE_TASK.h"
#include "MASK_CONNECTORS_STATE.hpp"
#include "Cache.hpp"
#include "EmergencyLightsAgent.hpp"
#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class LightModule
{
public:
	std::vector<MODULE_TASK*>* tasks;

	LightModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr);
	~LightModule();
	void initialize();
	void blink(int count);
	void changeConnectorStateIndication(std::string connectorId, std::string value);
	void setup();
	void handleTask(Obj* obj);
	int getModuleProtocol() { return static_cast<int>(bdmModuleObj_->protocol); }
private:
	boost::log::sources::logger_mt logger_;
	Cache* cachePtr_;
	EQM* eqmObjPtr_;
	std::vector<Obj*>* cache_;
	LIGHTES* lightes_;
	MODULE* bdmModuleObj_;
	std::map<std::string, std::vector<CONNECTOR*>> lampsConnectorsMap_;
	std::vector<CONNECTOR*> conns;
	int welcomeTaskSubscrId_;
	std::vector<int> doorsChangeSubscId_;
	DOORS doorsObj_;
	EmergencyLightsAgent* eLA_;

	void getBDMModules();
	void createLightsTopology();
	void createLightsObj();
	void createLightObjs();
	void changeLightProceduralState(std::string label, int value);
	std::string getShortLabelForPowerGroup(std::string label);
	void displayTopology();
	void changeConnectorStateHandler(CHANGE_CONNECTOR_STATE_TASK* task);
	void maskConnectorStateHandler(MASK_CONNECTORS_STATE* task);
	void handleDoorsStateChange(Obj* obj);
	void compareStates(Obj* obj);
};

