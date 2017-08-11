#pragma once
#include "Cache.hpp"
#include "LIGHTES.hpp"
#include "MODULE.hpp"
#include "CHANGE_BUTTON_STATE_IND.hpp"
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <boost\thread.hpp>
#include <vector>

class EmergencyLightsAgent
{
public:
	EmergencyLightsAgent(boost::log::sources::logger_mt logger, Cache* cachePtr);
	~EmergencyLightsAgent();
	void getBlinkers();
	std::vector<int> createMask();
private:
	boost::log::sources::logger_mt logger_;
	Cache* cachePtr_;
	bool bssf_;

	MODULE* lightModuleObjPtr_;
	std::vector<CONNECTOR*> blinkersCommonConns_;
	void handleChangeButtonStateIndication(Obj* obj);
	void startBlinkerService();
	void stopBlinkerService();
	
};

