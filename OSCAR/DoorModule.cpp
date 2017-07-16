#include "stdafx.h"
#include "DoorModule.hpp"


DoorModule::DoorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger)
{
	logger_ = logger;
	cache_ = cache;
	BOOST_LOG(logger_) << "DEBUG " << "DoorModule ctor";
	if (cache_ == nullptr)
		BOOST_LOG(logger_) << "ERROR " << "Cache cannot be setup ";
}


DoorModule::~DoorModule()
{
}

void DoorModule::initialize()
{
	BOOST_LOG(logger_) << "INFO " << "DoorModule::initialize";
	getBDMModules();
	prepareTopology();
	displayTopology();
	RESULT* res = new RESULT();
	res->applicant = "DOOR_MODULE";
	res->feedback = "Initialize";
	res->status = RESULT::EStatus::success;
	bdmModuleObj_->children.push_back(res);
}

void DoorModule::setup()
{
	BOOST_LOG(logger_) << "DBG " << "DoorModule::setup";
	std::vector<CONNECTOR*> wrongSetupConnectors;
	for (const auto &connGr : bdmModuleObj_->connectors_)
	{
		for (auto &conn : connGr)
		{
			auto connector = static_cast<CONNECTOR*>(conn);
			if (!commonGNDChecker::checkGNDConnVelue(connector))
			{
				wrongSetupConnectors.push_back(connector);
			}
			BOOST_LOG(logger_) << "DBG " << "DoorModule::setup: connector::Etype: " << static_cast<int>(connector->type);
			if (connector->type == CONNECTOR::EType::input)
				setDoorOpeningInitStatus(static_cast<DOOR::EOpeningState>(connector->value), connector->label);
			else if (connector->type == CONNECTOR::EType::output)
			{
				setDoorLockingInitStatus(static_cast<DOOR::ELockingState>(connector->value), connector->label);
			}
		}
	}
	bdmModuleObj_->protocol = MODULE::EProtocol::CSimpleMessage;
	bdmModuleObj_->operationalState = MODULE::EOperationalState::enabled;
	if (wrongSetupConnectors.size() == 0)
	{
		//should be result with status OK
	}
	else
	{
		//should be result with partial OK and feedback
	}
}

void DoorModule::checkAndExecuteTask()
{
	std::vector<MODULE_TASK*> tasks;
	if (!bdmModuleObj_->tasks.empty())
	{
		for (auto &task : bdmModuleObj_->tasks)
		{
			runTask(task);
		}
	}
}

void DoorModule::runTask(MODULE_TASK* task)
{
	if (task->name == MODULE_TASK::EName::CHANGE_CONNECTOR_STATE_TASK)
	{
		auto ccst = static_cast<CHANGE_CONNECTOR_STATE_TASK*>(task);
		auto feedback = changeConnectorState(ccst->port, ccst->value);
		if (feedback != boost::none)
		{
			ccst->result = new RESULT();
			ccst->result->applicant = "DoorModule";
			ccst->result->feedback = feedback.value();
			ccst->result->status = RESULT::EStatus::success;
			BOOST_LOG(logger_) << "INF " << "DoorModule::runTask: feedback: " << ccst->result->feedback;
		}
	}
}

void DoorModule::setDoorLockingInitStatus(DOOR::ELockingState lockState, std::string label)
{
	BOOST_LOG(logger_) << "INF " << "DoorModule::setDoorLockingInitStatus " << label << " " << static_cast<int>(lockState);
	auto common = doorsObj_->commonLockGND;
	for (auto &door : doorsObj_->container_)
	{
		BOOST_LOG(logger_) << "DBG " << "DoorModule::setDoorLockingInitState: door " << door->label << " has been found";
		if (door->label.find(label) != std::string::npos)
		{
			if (common->connectors[0]->value == 0)
			{
				door->lockingState = lockState;
				BOOST_LOG(logger_) << "INF " << "DoorModule::setDoorLockingInitStatus " << label << " " << static_cast<int>(lockState);
				return;
			}
			else 
			{
				door->lockingState = static_cast<DOOR::ELockingState>(!static_cast<int>(lockState));
				BOOST_LOG(logger_) << "INF " << "DoorModule::setDoorLockingInitStatus " << label << " " << static_cast<int>(lockState);
				return;
			}
			
		}
			
	}
}

void DoorModule::setDoorOpeningInitStatus(DOOR::EOpeningState openState, std::string label)
{
	for (auto &door : doorsObj_->container_)
	{
		if (door->label.find(label) != std::string::npos)
		{
			door->openingState = openState;
			BOOST_LOG(logger_) << "INF " << "DoorModule::setDoorOpeningInitStatus " << label << " " << static_cast<int>(openState);
			return;
		}

	}
	if (checkIfDoorsAreClosed() == nullptr)
		doorsObj_->openingState = DOORS::EOpeningState::closed;
	else
		doorsObj_->openingState = DOORS::EOpeningState::opened;	
}


void DoorModule::getBDMModules()
{
	for (const auto &obj : *cache_)
	{
		if (obj->name == "EQM")
		{
			for (const auto &mod : static_cast<EQM*>(obj)->modules_)
			{
				if (static_cast<MODULE*>(mod)->label.find("BDM_DOOR") != std::string::npos)
				{
					BOOST_LOG(logger_) << "INF " << "DoorModule::getBDMModule: MODULE found";
					bdmModuleObj_ = static_cast<MODULE*>(mod);
					return;
				}
					
			}
		}
	}
	BOOST_LOG(logger_) << "ERR " << "DoorModule::getBDMModule: MODULE not found";
}


void DoorModule::prepareTopology()
{
	doorsObj_ = new DOORS();
	std::vector<CONNECTOR*> connectors;
	for (const auto &connGr : bdmModuleObj_->connectors_)
	{
		for (const auto &conn : connGr)
		{
			if (static_cast<CONNECTOR*>(conn)->label.find("DOOR") != std::string::npos)
			{
				connectors.push_back(static_cast<CONNECTOR*>(conn));
			}
		}
	}
	if (connectors.size() == 0)
	{
		BOOST_LOG(logger_) << "INFO " << "DoorModule::prepareTopology: no connectors";
		return;
	}
	else
	{
		createDoors(connectors);
	}
		
	//displayTopology();
	cache_->push_back(doorsObj_);
}

void DoorModule::createDoors(std::vector<CONNECTOR*> connectors)
{
	BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors";
	std::vector<PORT*> ports;
	CONNECTOR* commonLockGNDConn;
	for (const auto &conn : connectors)
	{
		if (conn->label.find("GND") != std::string::npos)
			commonLockGNDConn = conn;
		BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors: connector label: " << conn->label;
		std::string label = [](std::string label)->std::string
		{
			std::vector<std::string> slabel;
			boost::split(slabel, label, boost::is_any_of("_"));
			if (slabel.size() == 2)
				return slabel[1];
			else if (slabel.size() == 4)
				return slabel[0] + "_" + slabel[2] + "_" + slabel[3];
			else
				return slabel[1] + "_" + slabel[2];
		}(conn->label);
		
		bool exist = [](std::string label, std::vector<PORT*> ports)-> bool
		{
			for (const auto &port : ports)
			{
				if (port->label == label)
					return true;
			}
			return false;
		}(label, ports);
		BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors check label exist: " << label << " " << exist;
		if (!exist)
		{
			BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors: port label does not exist. Creating : " << label;
			PORT* port = new PORT();
			port->label = label;
			port->connectors.push_back(conn);
			ports.push_back(port);
		}
		else
		{
			BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors: port label exist. Creating : " << label;
			for (const auto &port : ports)
			{
				if (port->label == label)
				{
					port->connectors.push_back(conn);
					break;
				}
			}
		}
	}
	for (const auto &port : ports)
	{
		if (!checkDoesDoorExist(port->label) && port->label.find("GND") == std::string::npos)
		{
			BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors: door label does not exist. Creating : " << port->label;
			DOOR* door = new DOOR(port->label);
			door->ports.push_back(port);
			doorsObj_->container_.push_back(door);
		}
		else if (port->label.find("GND") != std::string::npos)
		{
			doorsObj_->commonLockGND = port;
			doorsObj_->commonLockGND->connectors.push_back(commonLockGNDConn);
		}
		else
		{
			BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors: port label does exist.";
			for (const auto &door : doorsObj_->container_)
			{
				if (door->label == port->label)
				{
					door->ports.push_back(port);
					break;
				}
			}
		}
	}
	BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors: " << doorsObj_->container_.size() << " door has been created";
}

bool DoorModule::checkDoesDoorExist(std::string label)
{
	for (const auto &door : doorsObj_->container_)
	{
		if (door->label == label)
			return true;
	}
	return false;
}

void DoorModule::unlockDoors()
{
	BOOST_LOG(logger_) << "INFO " << "DoorModule::unlockDoors";
	for (auto &door : doorsObj_->container_)
	{
		BOOST_LOG(logger_) << "INFO " << "DoorModule::unlockDoors " << door->label;
		door->unlockDoor();
	}
	RESULT* result = new RESULT();
	result->status = RESULT::EStatus::success;
	result->applicant = "BDM_DOOR";
	result->feedback = std::to_string(doorsObj_->commonLockGND->connectors[0]->id) +
		std::to_string(doorsObj_->commonLockGND->connectors[0]->value);
	if (doorsObj_->commonLockGND->connectors[0]->value != 0)
	{
		doorsObj_->commonLockGND->connectors[0]->value = 0;
		result->feedback += std::to_string(doorsObj_->commonLockGND->connectors[0]->id)
			+ std::to_string(doorsObj_->commonLockGND->connectors[0]->value);
	}
	bdmModuleObj_->children.push_back(result);
}

void DoorModule::lockDoors()
{
	BOOST_LOG(logger_) << "INFO " << "DoorModule::lockDoors";
	std::string* label = checkIfDoorsAreClosed();
	if (label == nullptr && !checkIfBateryAlarmRaised() && doorsObj_->openingState != DOORS::EOpeningState::opened)
	{
		for (auto &door : doorsObj_->container_)
		{
			BOOST_LOG(logger_) << "INFO " << "DoorModule::lockDoors " << door->label;
			door->lockDoor();
		}
		return;
	}
	if (checkIfBateryAlarmRaised())
	{ 
		BOOST_LOG(logger_) << "WRN " << "DoorModule::lockDoors: doors cannot be lock due to battery low";
		return;
	}
	if (label != nullptr && !checkIfBateryAlarmRaised())
	{
		BOOST_LOG(logger_) << "WRN " << "DoorModule::lockDoors: doors cannot be lock due to opened " << *label << " door";
		return;
	}
	
}

std::string* DoorModule::checkIfDoorsAreClosed()
{
	for (const auto &door : doorsObj_->container_)
	{
		if (door->openingState == DOOR::EOpeningState::opened)
		{
			return new std::string(door->label);
		}
	}
	return nullptr;
}

bool DoorModule::checkIfBateryAlarmRaised()
{
	std::vector<ALARM*> alarms;
	for (const auto &obj : *cache_)
	{
		if (obj->name == "ALARM")
			alarms.push_back(static_cast<ALARM*>(obj));
	}
	if (!alarms.empty())
	{
		for (auto &alarm : alarms)
		{
			if (alarm->ref == "WCM" && alarm->alarmCode == 1001)
			{
				return true;
			}
		}
	}

	return false;
}


boost::optional<std::string> DoorModule::changeConnectorState(int connectorId, int value)
{
	BOOST_LOG(logger_) << "INF " << "DoorModule::changeConnectorState";
	for (const auto &door : doorsObj_->container_)		//refactor 3 fory?!
	{
		for (const auto &port : door->ports)
		{
			for ( auto &conn : port->connectors)
			{
				if (conn->id == connectorId)
				{
					BOOST_LOG(logger_) << "INF " << "DoorModule::changeConnectorState: on door: " << door->label
						<< " on port" << port->label << " connector ID: " << conn->id << " to value " << value;
					door->changeConnectorState(connectorId, value);
					changeDOORSOpeningStateIfNeeded(value);
					return boost::none;
				}
			}
		}
	}
	for (auto &conn : doorsObj_->commonLockGND->connectors)
	{
		if (conn->id == connectorId)
		{
			BOOST_LOG(logger_) << "INF " << "DoorModule::changeConnectorState: GND connector " << conn->id << " to value " << value;
			doorsObj_->setLockingState(value);
			return (value == 0) ? "LOCK_DOORS" : "UNLOCK_DOORS";
		}
	}
	BOOST_LOG(logger_) << "ERR " << "DoorModule::changeConnectorState: Connector not found...";
	return boost::none;
}

void DoorModule::changeDOORSOpeningStateIfNeeded(int value)
{
	bool diff = false;
	for (const auto &door : doorsObj_->container_)
	{
		if (door->openingState != static_cast<DOOR::EOpeningState>(value))
			diff = true;
	}
	if (!diff && doorsObj_->openingState != static_cast<DOORS::EOpeningState>(value))
	{
		doorsObj_->openingState = static_cast<DOORS::EOpeningState>(value);
		onOpen();
	}
}

void DoorModule::onOpen()
{

}

void DoorModule::onClose()
{

}

void DoorModule::displayTopology()
{
	BOOST_LOG(logger_) << "DEBUG " << "DoorModule::displayTopology";
	BOOST_LOG(logger_) << "DEBUG " << "DOORS_COMMON_LOCK_PORT " << doorsObj_->commonLockGND->label;
	BOOST_LOG(logger_) << "DEBUG " << "CONNECTOR " << doorsObj_->commonLockGND->connectors[0]->id << " " 
		<< doorsObj_->commonLockGND->connectors[0]->label;
	for (const auto &door : doorsObj_->container_)
	{
		BOOST_LOG(logger_) << "DEBUG " << door->label;
		for (const auto &port : door->ports)
		{
			BOOST_LOG(logger_) << "DEBUG " << "PORT " << port->label;
			for (const auto &conn : port->connectors)
			{
				BOOST_LOG(logger_) << "DEBUG " << "CONNECTOR " << conn->id << " " << static_cast<int>(conn->type);
			}
		}
		
	}
}
