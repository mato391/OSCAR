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
#include "RESULT.hpp"
#include "CONNECTORS_MASKING_DONE_IND.hpp"
#include "CHANGE_CONNECTOR_STATE_TASK.h"
#include "MASK_CONNECTORS_STATE.hpp"
#include "Cache.hpp"
#include "EmergencyLightsAgent.hpp"
#include "BeamLightAgent.hpp"

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
	void setup();
	int getModuleProtocol() { return static_cast<int>(bdmModuleObj_->protocol); }
private:
	boost::log::sources::logger_mt logger_;
	Cache* cachePtr_;
	std::vector<Obj*>* cache_;
	EQM* eqmObjPtr_;
	LIGHTES* lightes_;
	MODULE* bdmModuleObj_;
	DOORS doorsObj_;

	int cmdiSubscrId_;
	int cbsiSubscrId_;
	std::vector<int> doorsChangeSubscId_;	
	EmergencyLightsAgent* eLA_;
	BeamLightAgent* bLA_;
	std::vector<Obj*> connsVec_;

	void getBDMModules();
	void createLightsTopology();
	void createLightsObj();
	void createLightObjs(std::vector<std::string> labels);
	void changeLightProceduralState(std::string label, int value);
	void displayTopology();
	void handleDoorsStateChange(Obj* obj);
	void compareStates(Obj* obj);
	void handleIndication(Obj* obj);
	void setLightState(int connValue, int connId);
	void setLightStateBasedOnConnId(LIGHT* light, int connId, int connValue);
};

