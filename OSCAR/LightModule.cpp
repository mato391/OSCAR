#include "stdafx.h"
#include "LightModule.h"


LightModule::LightModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger)
{
	logger_ = logger;
	cache_ = cache;
	BOOST_LOG(logger_) << "DEBUG " << "LightModule ctor";
}


LightModule::~LightModule()
{
}

void LightModule::initialize()
{
	getCP();
	createLightsTopology();
	displayTopology();
}

void LightModule::getCP()
{
	for (const auto &obj : *cache_)
	{
		if (obj->name == "CP")
		{
			cpObj_ = static_cast<CP*>(obj);
			return;
		}
	}
	BOOST_LOG(logger_) << "ERROR " << "LightModule::getCP: There is no CP in cache";
}

void LightModule::createLightsTopology()
{
	BOOST_LOG(logger_) << "INFO " << "LightModule::createLightsTopology";
	lightes_ = new LIGHTES();
	createFrontLight(cpObj_->lightVersion);
	createCenterBlinkers(cpObj_->lightVersion);
	createBackLight(cpObj_->lightVersion);
	cache_->push_back(lightes_);
}


void LightModule::createFrontLight(int version)
{
	if (version >= 1)
	{
		lightes_->addLight(lightFactory("BEAM_FRONT_LEFT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("BEAM_FRONT_RIGHT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("LOW_BEAM_FRONT_LEFT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("LOW_BEAM_FRONT_RIGHT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("BLINKER_FRONT_LEFT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("BLINKER_FRONT_RIGHT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("POSITION_LEFT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("POSITION_RIGHT", LIGHT::EType::bulb));
		
	}
	if (version == 2)
	{
		lightes_->addLight(lightFactory("DAILY_LEFT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("DAILY_RIGHT", LIGHT::EType::bulb));
	}
	if (version == 3)
	{
		lightes_->addLight(lightFactory("BEAM_FRONT_LEFT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("BEAM_FRONT_RIGHT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("LOW_BEAM_FRONT_LEFT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("LOW_BEAM_FRONT_RIGHT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("BLINKER_FRONT_LEFT", LIGHT::EType::led));
		lightes_->addLight(lightFactory("BLINKER_FRONT_RIGHT", LIGHT::EType::led));
		lightes_->addLight(lightFactory("POSITION_LEFT", LIGHT::EType::led));
		lightes_->addLight(lightFactory("POSITION_RIGHT", LIGHT::EType::led));
		lightes_->addLight(lightFactory("DAILY_LEFT", LIGHT::EType::led));
		lightes_->addLight(lightFactory("DAILY_RIGHT", LIGHT::EType::led));

	}
}

void LightModule::createCenterBlinkers(int version)
{
	if (version >= 1)
	{
		lightes_->addLight(lightFactory("BLINKER_CENTER_LEFT", LIGHT::EType::bulb));
		lightes_->addLight(lightFactory("BLINKER_CENTER_RIGHT", LIGHT::EType::bulb));
	}
	else if (version == 3)
	{
		lightes_->addLight(lightFactory("BLINKER_CENTER_LEFT", LIGHT::EType::led));
		lightes_->addLight(lightFactory("BLINKER_CENTER_RIGHT", LIGHT::EType::led));
	}
	
}

void LightModule::createBackLight(int version)
{
	LIGHT::EType type;
	if (version >= 1)
		type = LIGHT::EType::bulb;
	if (version == 3)
		type = LIGHT::EType::led;
	else
		BOOST_LOG(logger_) << "WARNING " << "LightModule::createBackLight: Unknown type";

	lightes_->addLight(lightFactory("BACK_LOW_BEAM_LEFT", type));
	lightes_->addLight(lightFactory("BACK_LOW_BEAM_RIGHT", type));
	lightes_->addLight(lightFactory("STOP_LEFT", type));
	lightes_->addLight(lightFactory("STOP_CENTER", type));
	lightes_->addLight(lightFactory("STOP_RIGHT", type));
	lightes_->addLight(lightFactory("FOG", LIGHT::EType::bulb));
	lightes_->addLight(lightFactory("REVERSE", LIGHT::EType::bulb));

}

LIGHT* LightModule::lightFactory(std::string label, LIGHT::EType type)
{
	LIGHT* light = new LIGHT();
	light->label = label;
	light->type = type;
	return light;
}

void LightModule::blink()
{
	for (auto &light : lightes_->container_)
	{
		if (light->label.find("BLINKER") != std::string::npos)
		{
			BOOST_LOG(logger_) << "INFO " << "LightModule::blink";
			//Should be added calling hwapi message
			if (light->proceduralState == LIGHT::EProceduralState::on)
				light->proceduralState = LIGHT::EProceduralState::off;
			else
				light->proceduralState = LIGHT::EProceduralState::on;
		}
	}
}

void LightModule::displayTopology()
{
	BOOST_LOG(logger_) << "DEBUG " << "LightModule::displayTopology";
	for (const auto &light : lightes_->container_)
	{
		BOOST_LOG(logger_) << "DEBUG " << light->label << " " << static_cast<int>(light->type);
	}
}