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
	void initialize();
private:
	boost::log::sources::logger_mt logger_;
	Cache* cachePtr_;
	MODULE* lightModulePtr_;
	std::vector<Obj*> lightModulesConnsObjs_;
	std::vector<LIGHT*> posLightObjs_;
	std::vector<LIGHT*> beamLightObjs_;
	int cbsiSubscrId_;

	
	void getLightModule();
	std::vector<LIGHT*> getLightsByLabel(std::string label);
	void changeButtonStateHandler(Obj* obj);
	void setInitialStates();
	bool checkConnectorsState(std::vector<int> refs);
	CONNECTOR* getConnectorByRef(int ref);
	



};

