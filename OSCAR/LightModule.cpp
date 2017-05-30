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
	BOOST_LOG(logger_) << "INF " << "LightModule::initialize";
	getCP();
	getBDMModules();
	createLightsTopology();
	createLightsObj();
	//displayTopology();
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

void LightModule::getBDMModules()
{
	for (const auto &obj : *cache_)
	{
		if (obj->name == "EQM")
		{
			for (const auto &mod : static_cast<EQM*>(obj)->modules_)
			{
				if (static_cast<MODULE*>(mod)->label == "BDM_LIGHT")
				{
					BOOST_LOG(logger_) << "INF " << "LightModule::getBDMModule: MODULE found";
					bdmModuleObj_ = static_cast<MODULE*>(mod);
					return;
				}

			}
		}
	}
	BOOST_LOG(logger_) << "ERR " << "LightModule::getBDMModule: MODULE not found";
}

void LightModule::createLightsObj()
{
	lightes_ = new LIGHTES();
}

void LightModule::createLightsTopology()
{
	BOOST_LOG(logger_) << "INFO " << "LightModule::createLightsTopology";
	/*lightes_ = new LIGHTES();
	createFrontLight(cpObj_->lightVersion);
	createCenterBlinkers(cpObj_->lightVersion);
	createBackLight(cpObj_->lightVersion);
	cache_->push_back(lightes_);*/

	std::vector<std::string> labels;
	for (const auto &vec : bdmModuleObj_->connectors_)
	{
		for (const auto &obj : vec)
		{
			auto conn = static_cast<CONNECTOR*>(obj);
			BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology for connector: " << conn->id << " " << conn->label;
			if (conn->label.find("LIGHT") != std::string::npos)
			{
				conns.push_back(conn);
				BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology adding conn: " << conn->id << " " << conn->label;
			}
				
		}
	}
	for (auto &conn : conns)
	{
		std::vector<std::string> sconnLabel;
		boost::split(sconnLabel, conn->label, boost::is_any_of("_"));
		bool isAlready = [](std::vector<std::string>* labelsVec, std::string connLab)->bool
		{
			std::cout << "I have label: " << connLab << std::endl;
			if (labelsVec->empty())
			{
				std::cout << "LABELS vec is empty returning false" << std::endl;
				return false;
			}
				
			for (const auto &label : *labelsVec)
			{
				std::cout << "LABELS vec is not empty checking: " << label << std::endl;
				if (label.find(connLab) != std::string::npos)
				{
					std::cout << "LABEL exist returning true" << std::endl;
					return true;
				}
			}
			std::cout << "LABEL does not exist returning false" << std::endl;
			return false;
		}(&labels, sconnLabel[1]);
		if (!isAlready)
		{
			//BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology adding label: " << conn->label;
			labels.push_back(conn->label);
		}
		//else
			//BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology is already added: " << conn->label;
			
	}
	createLightsObj();
	for (const auto &label : labels)
	{
		//BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology creating powerGroup: " << label << " with shortcut "
//				<< getShortLabelForPowerGroup(label);
		lightes_->powerGroups_.push_back(new POWER_GROUP(
			getShortLabelForPowerGroup(label)));
	}
	createLightObjs();
}

std::string LightModule::getShortLabelForPowerGroup(std::string label)
{
	std::vector<std::string> splittedLabel;
	boost::split(splittedLabel, label, boost::is_any_of("_"));
	return splittedLabel[1];
}


void LightModule::createLightObjs()
{
	for (const auto &conn : conns)
	{
		//BOOST_LOG(logger_) << "DBG " << "createLightObjs for connector: " << conn->id << " " << conn->label;
		std::string lightLabel = [](std::string label)->std::string
		{
			std::vector<std::string> splittedLabel;
			boost::split(splittedLabel, label, boost::is_any_of("_"));
			return splittedLabel [1] + "_" + splittedLabel[2] + "_" + splittedLabel[3];
		}(conn->label);
		//BOOST_LOG(logger_) << "DBG " << "createLightObjs check is light exist: " << lightLabel;
		auto isAlreadyCreated = [](std::string label, LIGHTES* lightsObjPtr)->std::pair<std::string, POWER_GROUP*>
		{
			std::vector<std::string> splittedLabel;
			boost::split(splittedLabel, label, boost::is_any_of("_"));
			for (const auto &powerGroup : lightsObjPtr->powerGroups_)
			{
				if (powerGroup->label == splittedLabel[0])
				{
					for (const auto &light : powerGroup->lights_)
					{
						if (light->label == label)
							return std::make_pair("EXIST", powerGroup);
					}
					return std::make_pair("NOT_EXIST", powerGroup);
				}
				return std::make_pair("NOT_EXIST", nullptr);
			}
		}(lightLabel, lightes_);
		//BOOST_LOG(logger_) << "DBG " << "isAlreadtCreated: " << isAlreadyCreated.first << " " << isAlreadyCreated.second;
		if (isAlreadyCreated.first == "NOT_EXIST" && isAlreadyCreated.second != nullptr)
		{
			LIGHT* light = new LIGHT();
			light->label = lightLabel;
			light->proceduralState = LIGHT::EProceduralState::on;//Should be setup based on connector state;
			light->connector = conn;
			isAlreadyCreated.second->lights_.push_back(light);
		}
	}
	displayTopology();
}

void LightModule::createFrontLight(int version)
{
	/*if (version >= 1)
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

	}*/
	
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
/*void LightModule::blink()
{
	for (auto &pg : lightes_->powerGroups_)
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
}*/


void LightModule::displayTopology()
{
	BOOST_LOG(logger_) << "DEBUG " << "LightModule::displayTopology";
	for (const auto &powerGroup : lightes_->powerGroups_)
	{
		BOOST_LOG(logger_) << "DEBUG " << "POWER_GROUP: "  << powerGroup->label;
		for (const auto &light : powerGroup->lights_)
		{
			BOOST_LOG(logger_) << "DEBUG " << "LIGHT: " << light->label;
			BOOST_LOG(logger_) << "DEBUG " << "CONNECTOR: " << light->connector->id << " " << light->connector->label;
		}
	}
}