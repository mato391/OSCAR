#pragma once
#include <vector>
#include <iostream>
#include <map>

#include "Objects\Obj.hpp"
#include "Objects\CP.hpp"
#include "Objects\LIGHT.hpp"
#include "LIGHTES.hpp"
#include "MODULE.hpp"
#include "EQM.hpp"
#include "POWER_GROUP.hpp"
#include "RESULT.hpp"

#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class LightModule
{
public:
	LightModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger);
	~LightModule();
	void initialize();
	void blink(int count);
	void changeConnectorStateIndication(std::string connectorId, std::string value);
	void setup();
private:
	boost::log::sources::logger_mt logger_;
	CP* cpObj_;
	EQM* eqmObjPtr_;
	std::vector<Obj*>* cache_;
	LIGHTES* lightes_;
	MODULE* bdmModuleObj_;
	std::map<std::string, std::vector<CONNECTOR*>> lampsConnectorsMap_;

	void getCP();
	void getBDMModules();
	void createLightsTopology();
	void createLightsObj();
	void createLightObjs();
	void changeLightProceduralState(std::string label, int value);
	std::string getShortLabelForPowerGroup(std::string label);
	LIGHT* lightFactory(std::string label, LIGHT::EType type);
	void displayTopology();
	std::vector<CONNECTOR*> conns;
	
};

