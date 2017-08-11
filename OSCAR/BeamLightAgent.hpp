#pragma once
#include "CONNECTOR.hpp"
#include "CHANGE_BUTTON_STATE_IND.hpp"
#include "Objects\LIGHT.hpp"
#include "MODULE.hpp"

#include "Cache.hpp"

#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class BeamLightAgent
{
public:
	BeamLightAgent(boost::log::sources::logger_mt logger, Cache* cachePtr);
	~BeamLightAgent();
private:
	boost::log::sources::logger_mt logger_;
	Cache* cachePtr_;
	MODULE* lightModulePtr_;
	std::vector<CONNECTOR*> beamCommonConns_;
	std::vector<LIGHT*> beamLightObjs_;

	void initialize();
	void getLightModule();
	void getBeamConnectors();
	void getBeamLights();

};

