#include "stdafx.h"
#include "BeamLightAgent.hpp"


BeamLightAgent::BeamLightAgent(boost::log::sources::logger_mt logger, Cache* cachePtr) : logger_(logger), cachePtr_(cachePtr)
{
	initialize();
}


BeamLightAgent::~BeamLightAgent()
{
}

void BeamLightAgent::initialize()
{
	getLightModule();
	getBeamConnectors();
	getBeamLights();
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

void BeamLightAgent::getBeamConnectors()
{
	if (lightModulePtr_ == nullptr)
	{
		BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " lightModule not found";
		return;
	}
	auto connsVec = cachePtr_->getAllObjectsUnder(lightModulePtr_, "CONNECTOR");
	for (const auto &obj : connsVec)
	{
		auto connector = static_cast<CONNECTOR*>(obj);
		if (connector->label.find("BEAM") != std::string::npos
			&& connector->label.find("GND") != std::string::npos)
		{
			beamCommonConns_.push_back(connector);
		}
	}
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " beamCommonConns size: " << beamCommonConns_.size();
}

void BeamLightAgent::getBeamLights()
{
	if (lightModulePtr_ == nullptr)
	{
		BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " lightModule not found";
		return;
	}
	auto lights = cachePtr_->getUniqueObjectUnder(lightModulePtr_, "LIGHTES");
	if (lights == nullptr)
	{
		BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " LIGHTES does not exist";
		return;
	}
	auto pgsVec = cachePtr_->getAllObjectsUnder(lights, "POWER_GROUP");
	for (const auto &obj : pgsVec)
	{
		for (const auto &light : obj->children)
		{
			if (light->name == "LIGHT" )
			{
				auto lightC = static_cast<LIGHT*>(light);
				if (lightC->label.find("BEAM") != std::string::npos)
					beamLightObjs_.push_back(lightC);
			}
		}
	}
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " beamLightObjs size: " << beamLightObjs_.size();
}