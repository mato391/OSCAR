#include "stdafx.h"
#include "StartStopEngineButtonAgent.hpp"


StartStopEngineButtonAgent::StartStopEngineButtonAgent(SWITCH_TOPOLOGY* swTop, boost::log::sources::logger_mt logger) :
	swTopPtr_(swTop),
	logger_(logger)
{
	BOOST_LOG(logger_) << "INFO " << "StartStopEngineButtonAgent ctor";
}


StartStopEngineButtonAgent::~StartStopEngineButtonAgent()
{
}

RESULT StartStopEngineButtonAgent::buttonPushedAction()
{
	RESULT* result = new RESULT();
	result->applicant = "StartStopEngineButtonAgent";
	startStopEngineButtonObjPtr_ = static_cast<BUTTON*>(getObjectFromTopology("START_STOP_ENGINE", "BUTTON"));
	auto stopPedalObjPtr = static_cast<PEDAL*>(getObjectFromTopology("PEDAL_STOP", "PEDAL"));
	if (stopPedalObjPtr->isPushed)
	{
		BOOST_LOG(logger_) << "INFO " << "StartStopEngineButtonAgent::buttonPushedAction: success";
		result->status = RESULT::EStatus::success;
		return *result;
	}
	else
	{	
		result->status = RESULT::EStatus::failed;
		result->feedback = "CAR is not stopped";
		BOOST_LOG(logger_) << "INFO " << "StartStopEngineButtonAgent::buttonPushedAction: failed with feedback: " << result->feedback;
		return *result;
	}
	return *result;

}

Obj* StartStopEngineButtonAgent::getObjectFromTopology(std::string label, std::string type)
{
	
	if (type == "BUTTON")
	{
		for (const auto &obj : swTopPtr_->buttonTopology)
		{
			if (static_cast<BUTTON*>(obj)->label == label)
				return obj;
		}
	}
	else if (type == "SWITCH")
	{

	}
	else if (type == "PEDAL")
	{
		for (const auto &obj : swTopPtr_->pedals)
		{
			if (static_cast<PEDAL*>(obj)->label == label)
				return obj;
		}
	}
	return nullptr;
	
}

RESULT* StartStopEngineButtonAgent::getOperationResultAndReturnOperStatus()
{
	for (auto &obj : engineObjPtr_->children)
	{
		if (obj->name == "RESULT")
		{
			return static_cast<RESULT*>(obj);
		}
	}
}