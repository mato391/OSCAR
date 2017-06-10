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

void LightModule::setup()
{
	for (const auto &connGr : bdmModuleObj_->connectors_)
	{
		for (auto &conn : connGr)
		{
			auto connector = (static_cast<CONNECTOR*>(conn));
			changeLightProceduralState(connector->label, connector->value);
		}
	}
}

void LightModule::changeLightProceduralState(std::string label, int value)
{
	for (const auto &pg : lightes_->powerGroups_)
	{
		for (auto &light : pg->lights_)
		{
			auto common = pg->commonGND;
			if (label.find(light->label) != std::string::npos)
			{
				if (common->value == 0)
					light->proceduralState = static_cast<LIGHT::EProceduralState>(value);
				else
					light->proceduralState = static_cast<LIGHT::EProceduralState>(!value);
			}
		}
	}
}

void LightModule::initialize()
{
	BOOST_LOG(logger_) << "INF " << "LightModule::initialize";
	getCP();
	getBDMModules();
	createLightsTopology();
	//displayTopology();
}

void LightModule::changeConnectorStateIndication(std::string connectorId, std::string value)
{
	for (const auto &pg : lightes_->powerGroups_)
	{
		for (auto &light : pg->lights_)
		{
			if (light->connector->id == std::stoi(connectorId))
			{
				light->proceduralState = static_cast<LIGHT::EProceduralState>(std::stoi(value));
				return;
			}
				
		}
		if (pg->commonGND->id == std::stoi(connectorId))
		{
			for (auto &light : pg->lights_)
			{
				light->proceduralState = static_cast<LIGHT::EProceduralState>(std::stoi(value));
				return;
			}
		}
	}
	
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
			BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology adding label: " << conn->label;
			labels.push_back(conn->label);
		}
		else
			BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology is already added: " << conn->label;
			
	}
	createLightsObj();
	for (const auto &label : labels)
	{
		BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology creating powerGroup: " << label << " with shortcut "
				<< getShortLabelForPowerGroup(label);
		lightes_->powerGroups_.push_back(new POWER_GROUP(
			getShortLabelForPowerGroup(label)));
	}
	createLightObjs();
	bdmModuleObj_->children.push_back(lightes_);
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
		BOOST_LOG(logger_) << "DBG " << "createLightObjs for connector: " << conn->id << " " << conn->label;
		std::string lightLabel = [](std::string label)->std::string
		{
			std::vector<std::string> splittedLabel;
			boost::split(splittedLabel, label, boost::is_any_of("_"));
			return splittedLabel [1] + "_" + splittedLabel[2] + "_" + splittedLabel[3];
		}(conn->label);
		BOOST_LOG(logger_) << "DBG " << "createLightObjs check is light exist: " << lightLabel;
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
				
			}
			return std::make_pair("NOT_EXIST", nullptr);
		}(lightLabel, lightes_);
		BOOST_LOG(logger_) << "DBG " << "isAlreadyCreated: " << isAlreadyCreated.first << " " << isAlreadyCreated.second;
		if (isAlreadyCreated.first == "NOT_EXIST" && isAlreadyCreated.second != nullptr && conn->label.find("GND") == std::string::npos)
		{
			LIGHT* light = new LIGHT();
			light->label = lightLabel;
			light->proceduralState = LIGHT::EProceduralState::on;//Should be setup based on connector state;
			light->connector = conn;
			isAlreadyCreated.second->lights_.push_back(light);
		}
		else if (conn->label.find("GND") != std::string::npos && isAlreadyCreated.first == "NOT_EXIST" && isAlreadyCreated.second != nullptr)
		{
			BOOST_LOG(logger_) << "DBG " << "LightModule::createLightObjs: Adding common GND conn";
			isAlreadyCreated.second->commonGND = conn;
		}
	}
	displayTopology();
}



LIGHT* LightModule::lightFactory(std::string label, LIGHT::EType type)
{
	LIGHT* light = new LIGHT();
	light->label = label;
	light->type = type;
	return light;
}

void LightModule::blink(int count)
{
	std::vector<CONNECTOR*> commonGNDs;
	BOOST_LOG(logger_) << "DBG " << "LightModule::blink: powerGroup label: " << lightes_->powerGroups_.size();
	for (const auto &pg : lightes_->powerGroups_)
	{
		if (pg->label.find("BLINKER") != std::string::npos)
		{
			BOOST_LOG(logger_) << "DBG " << "LightModule::blink: powerGroup label: " << pg->commonGND->label;
			commonGNDs.push_back(pg->commonGND);
		}
	}
	BOOST_LOG(logger_) << "DBG " << "LightModule::blink: commonGND founded: " << commonGNDs.size();
	RESULT* result = new RESULT();
	result->applicant = bdmModuleObj_->label;
	result->status = RESULT::EStatus::success;
	result->feedback = commonGNDs[0]->id + commonGNDs[1]->id;
	bdmModuleObj_->children.push_back(result);
}


void LightModule::displayTopology()
{
	BOOST_LOG(logger_) << "DEBUG " << "LightModule::displayTopology";
	for (const auto &powerGroup : lightes_->powerGroups_)
	{
		BOOST_LOG(logger_) << "DEBUG " << "POWER_GROUP: "  << powerGroup->label;
		if (powerGroup->commonGND != nullptr)
			BOOST_LOG(logger_) << "DEBUG " << "COMMON_GND: " << powerGroup->commonGND->id << " " << powerGroup->commonGND->label;
		for (const auto &light : powerGroup->lights_)
		{
			BOOST_LOG(logger_) << "DEBUG " << "LIGHT: " << light->label;
			BOOST_LOG(logger_) << "DEBUG " << "CONNECTOR: " << light->connector->id << " " << light->connector->label;
		}
	}
}