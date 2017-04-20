#pragma once
#include "SWITCH_TOPOLOGY.hpp"
#include "PEDAL.h"
#include "RESULT.hpp"
#include "Component.hpp"
#include "INTER_MODULE_OPERATION.h"
#include "EDM.hpp"
#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class StartStopEngineButtonAgent
{
public:
	StartStopEngineButtonAgent(SWITCH_TOPOLOGY* swTop, boost::log::sources::logger_mt logger);
	~StartStopEngineButtonAgent();
	RESULT buttonPushedAction();
private:
	SWITCH_TOPOLOGY* swTopPtr_;
	boost::log::sources::logger_mt logger_;
	BUTTON* startStopEngineButtonObjPtr_;
	Obj* getObjectFromTopology(std::string label, std::string type);
	RESULT* getOperationResultAndReturnOperStatus();
};

