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
	void blink();
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
	void createLowestVersionLightTopology();
	void createFrontLight(int version);
	void createCenterBlinkers(int version);
	void createBackLight(int version);
	LIGHT* lightFactory(std::string label, LIGHT::EType type);
	void displayTopology();
	
};

