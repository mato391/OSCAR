#include "stdafx.h"
#include "LightModule.h"


LightModule::LightModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr)
{
	cachePtr_ = cachePtr;
	logger_ = logger;
	cache_ = cache;
	doorsObj_ = DOORS();
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
	welcomeTaskSubscrId_ = cachePtr_->subscribe("MODULE_TASK", std::bind(&LightModule::handleTask, this, std::placeholders::_1));
	int doorsChangeSubscId = cachePtr_->subscribe("DOORS", std::bind(&LightModule::handleDoorsStateChange, this, std::placeholders::_1));
}

void LightModule::handleDoorsStateChange(Obj* obj)
{
	if (doorsObj_.children.empty())
		doorsObj_ = *(static_cast<DOORS*>(obj));
	else
	{
		compareStates(obj);
		doorsObj_ = *(static_cast<DOORS*>(obj));
	}
}

void LightModule::compareStates(Obj* obj)
{
	auto doorsObj = static_cast<DOORS*>(obj);
	if (doorsObj_.openingState != doorsObj->openingState)
	{
		BOOST_LOG(logger_) << "INF " << "LightModule::compareStates " << "openingState has been changed";
	}
	else if (doorsObj_.lockingState != doorsObj->lockingState)
	{
		BOOST_LOG(logger_) << "INF " << "LightModule::compareStates " << "lockingState has been changed";
		auto res = new RESULT();
		res->applicant = "LIGHT_MODULE";
		res->feedback = getCommonGndConnectorId("BLINKER") + ":" + "1:" + std::to_string(static_cast<int>(doorsObj->lockingState) + 1) + "5";
		res->status = RESULT::EStatus::success;
		res->type = RESULT::EType::executive;
		cachePtr_->addToChildren(bdmModuleObj_, res);
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

void LightModule::handleTask(Obj* obj)
{
	auto res = new RESULT();
	res->applicant = "LIGHT_MODULE";
	auto moduleTask = static_cast<MODULE_TASK*>(obj);
	if (moduleTask->taskFor == bdmModuleObj_->domain)
	{
		if (moduleTask->type == MODULE_TASK::EName::LIGHT_WELCOMING_TASK)
		{
			BOOST_LOG(logger_) << "INF " << "LightModule::handleTask: LIGHT_WELCOMING_TASK";

			res->feedback = getCommonGndConnectorId("BLINKER") + ":" + "1" + ":" "15";
			res->status = RESULT::EStatus::success;
			res->type = RESULT::EType::executive;

		}
		else if (moduleTask->type == MODULE_TASK::EName::LIGHT_GOODBYE_TASK)
		{
			BOOST_LOG(logger_) << "INF " << "LightModule::handleTask: LIGHT_GOODBYE_TASK";
			res->feedback = getCommonGndConnectorId("BLINKER") + ":" + "1" + ":" "25";
			res->status = RESULT::EStatus::success;
			res->type = RESULT::EType::executive;
		}
		else if (moduleTask->type == MODULE_TASK::EName::CHANGE_CONNECTOR_STATE_TASK)
		{
			BOOST_LOG(logger_) << "INF " << "LightModule::handleTask: CHANGE_CONNECTOR_STATE_TASK";
			changeConnectorStateHandler(static_cast<CHANGE_CONNECTOR_STATE_TASK*>(moduleTask));
		}
		cachePtr_->addToChildren(moduleTask, res);
	}
	else
		BOOST_LOG(logger_) << "INF " << "LightModule::handleTask " << "This task is not for LightModule or type is unknown " << moduleTask->taskFor << " != " << bdmModuleObj_->domain;
	
}

void LightModule::changeConnectorStateHandler(CHANGE_CONNECTOR_STATE_TASK* task)
{
	BOOST_LOG(logger_) << "INF " << "LightModule::changeConnectorStateHandler";
	std::string pgLabel;
	for (auto &pg : lightes_->powerGroups_)
	{
		if (pg->commonGND->id == task->port)
		{
			pg->commonGND->value = task->value;
			pgLabel = pg->label;
			break;
		}
	}
	if (pgLabel.find("BLINKER") != std::string::npos)
	{
		BOOST_LOG(logger_) << "INF " << "LightModule::changeConnectorStateHandler: blinkers has been changed to: " << task->value;
	}
	else
		BOOST_LOG(logger_) << "ERR " << "LightModule::changeConnectorStateHandler: No pg found :(";
}


void LightModule::getBDMModules()
{
	auto bdmModules = cachePtr_->getAllObjectsFromChildren("EQM", "MODULE");
	for (const auto &obj : bdmModules)
	{
		if (static_cast<MODULE*>(obj)->label == "BDM_LIGHT")
		{
			BOOST_LOG(logger_) << "INF " << "LightModule::getBDMModule: MODULE found";
			bdmModuleObj_ = static_cast<MODULE*>(obj);
			return;
		}
	}
	BOOST_LOG(logger_) << "ERR " << "LightModule::getBDMModule: MODULE not found";
}

void LightModule::createLightsObj()
{
	lightes_ = new LIGHTES();
	cachePtr_->addToChildren(bdmModuleObj_, lightes_);
}

void LightModule::createLightsTopology()
{
	BOOST_LOG(logger_) << "INFO " << "LightModule::createLightsTopology";
	std::vector<std::string> labels;
	auto connsVec = cachePtr_->getAllObjectsUnder(bdmModuleObj_, "CONNECTOR");
	for (const auto &obj : connsVec)
	{
		auto conn = static_cast<CONNECTOR*>(obj);
		BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology for connector: " << conn->id << " " << conn->label;
		if (conn->label.find("LIGHT") != std::string::npos)
		{
			conns.push_back(conn);
			BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology adding conn: " << conn->id << " " << conn->label;
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
	cachePtr_->addToChildren(bdmModuleObj_, result);
	//bdmModuleObj_->children.push_back(result);
}

std::string LightModule::getCommonGndConnectorId(std::string label)
{
	for (const auto &pg : lightes_->powerGroups_)
	{
		if (pg->label.find(label) != std::string::npos)
		{
			return std::to_string(pg->commonGND->id);
		}
	}
	return "";
			
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