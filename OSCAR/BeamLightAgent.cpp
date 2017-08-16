#include "stdafx.h"
#include "BeamLightAgent.hpp"


BeamLightAgent::BeamLightAgent(boost::log::sources::logger_mt logger, Cache* cachePtr) : logger_(logger), cachePtr_(cachePtr)
{
}


BeamLightAgent::~BeamLightAgent()
{
}

void BeamLightAgent::initialize()
{
	getLightModule();
	lightModulesConnsObjs_ = cachePtr_->getAllObjectsUnder(lightModulePtr_, "CONNECTOR");
	beamLightObjs_ = getLightsByLabel("BEAM");
	posLightObjs_ = getLightsByLabel("POS");
	setInitialStates();
	cbsiSubscrId_ = cachePtr_->subscribe("CHANGE_BUTTON_STATE_IND", std::bind(&BeamLightAgent::changeButtonStateHandler, this, std::placeholders::_1), { 0 })[0];
}

void BeamLightAgent::setInitialStates()
{
	for (const auto &light : beamLightObjs_)
	{
		if (!checkConnectorsState(light->refs))
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " LIGHT changing proceduralState to off";
			light->proceduralState = LIGHT::EProceduralState::off;
		}
		else
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " LIGHT changing proceduralState to on";
			light->proceduralState = LIGHT::EProceduralState::on;
		}

	}
	for (const auto &light : posLightObjs_)
	{
		if (!checkConnectorsState(light->refs))
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " LIGHT changing proceduralState to off";
			light->proceduralState = LIGHT::EProceduralState::off;
		}
		else
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " LIGHT changing proceduralState to on";
			light->proceduralState = LIGHT::EProceduralState::on;
		}

	}
}

bool BeamLightAgent::checkConnectorsState(std::vector<int> refs)
{
	if (refs.size() != 2)
	{
		BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " wrong configuration";
		return false;
	}
	std::vector<CONNECTOR*> connsFromRef;
	for (const auto &ref : refs)
	{
		connsFromRef.push_back(getConnectorByRef(ref));
	}
	if (connsFromRef.size() != 2)
	{
		BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " wrong configuration";
		return false;
	}
	bool isGNDOff = false;
	bool isDedicatedOn = false;
	for (const auto &conn : connsFromRef)
	{
		if (conn->label.find("GND") != std::string::npos 
			&& conn->value == 0)
		{
			BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << "GND is 0";
			isGNDOff = true;
		}
		else if (conn->label.find("GND") == std::string::npos
			&& conn->value == 1)
		{
			BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << "Dedicated is 1";
			isDedicatedOn = true;
		}
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connLabel: "
			<< conn->label << " isGDNOff " << isGNDOff;
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connLabel: "
			<< conn->label << " isDedicatedOn " << isDedicatedOn;
	}
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " returning: " << (isGNDOff && isDedicatedOn);
	return (isGNDOff && isDedicatedOn);
}

CONNECTOR* BeamLightAgent::getConnectorByRef(int ref)
{
	for (const auto &obj : lightModulesConnsObjs_)
	{
		auto conn = static_cast<CONNECTOR*>(obj);
		if (conn->id == ref)
			return conn;
	}
	return nullptr;
}

void BeamLightAgent::getLightModule()
{
	auto modulesVec = cachePtr_->getAllObjectsFromChildren("EQM", "MODULE");
	for (const auto &obj : modulesVec)
	{
		auto module = static_cast<MODULE*>(obj);
		if (module->label == "BDM_LIGHT")
		{
			lightModulePtr_ = module;
		}
	}
}

std::vector<LIGHT*> BeamLightAgent::getLightsByLabel(std::string label)
{
	std::vector<LIGHT*> tmp;
	if (lightModulePtr_ == nullptr)
	{
		BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " lightModule not found";
		return {};
	}
	auto lights = cachePtr_->getUniqueObjectUnder(lightModulePtr_, "LIGHTES");
	if (lights == nullptr)
	{
		BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " LIGHTES does not exist";
		return {};
	}
	for (const auto &light : lights->children)
	{
		if (light->name == "LIGHT")
		{
			auto lightC = static_cast<LIGHT*>(light);
			if (lightC->label.find(label) != std::string::npos)
				tmp.push_back(lightC);
		}
	}
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " beamLightObjs size: " << beamLightObjs_.size();
	return tmp;
}

void BeamLightAgent::changeButtonStateHandler(Obj* obj)
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	auto cbsi = static_cast<CHANGE_BUTTON_STATE_IND*>(obj);
	if (cbsi->buttonLabel.find("LIGHT_SWITCH") != std::string::npos)
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " LIGHT_SWITCH in pos " << cbsi->value;
		if (cbsi->value == 1 && beamLightObjs_[0]->proceduralState == LIGHT::EProceduralState::on)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " TURNING OFF all Lights";
		}
		else if (cbsi->value == 1
			&& beamLightObjs_[0]->proceduralState == LIGHT::EProceduralState::off
			&& posLightObjs_[0]->proceduralState == LIGHT::EProceduralState::on)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " TURNING OFF Position Lights";
		}
		else if (cbsi->value == 2
			&& beamLightObjs_[0]->proceduralState == LIGHT::EProceduralState::on
			&& posLightObjs_[0]->proceduralState == LIGHT::EProceduralState::on)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " TURNING OFF Beam Lights";
		}
		else if (cbsi->value == 2
			&& beamLightObjs_[0]->proceduralState == LIGHT::EProceduralState::off
			&& posLightObjs_[0]->proceduralState == LIGHT::EProceduralState::off)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " TURNING ON Position Lights";
		}
		else if (cbsi->value == 3
			&& beamLightObjs_[0]->proceduralState == LIGHT::EProceduralState::off
			&& posLightObjs_[0]->proceduralState == LIGHT::EProceduralState::off)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " TURNING ON All Lights";
		}
		else if (cbsi->value == 3
			&& beamLightObjs_[0]->proceduralState == LIGHT::EProceduralState::off
			&& posLightObjs_[0]->proceduralState == LIGHT::EProceduralState::on)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " TURNING ON Beam Lights";
		}
	}
}