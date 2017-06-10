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
	getCP();
	getBDMModules();
	prepareTopology();
	displayTopology();
}

void DoorModule::setup()
{
	BOOST_LOG(logger_) << "DBG " << "DoorModule::setup";
	for (const auto &connGr : bdmModuleObj_->connectors_)
	{
		for (auto &conn : connGr)
		{
			auto connector = static_cast<CONNECTOR*>(conn);
			BOOST_LOG(logger_) << "DBG " << "DoorModule::setup: connector::Etype: " << static_cast<int>(connector->type);
			if (connector->type == CONNECTOR::EType::input)
				setDoorOpeningInitStatus(static_cast<DOOR::EOpeningState>(connector->value), connector->label);
			else if (connector->type == CONNECTOR::EType::output)
			{
				setDoorLockingInitStatus(static_cast<DOOR::ELockingState>(connector->value), connector->label);
			}
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
			int state = static_cast<int>(lockState) * common->connectors[0]->value;
			door->lockingState = static_cast<DOOR::ELockingState>(state);
			BOOST_LOG(logger_) << "INF " << "DoorModule::setDoorLockingInitStatus " << label << " " << static_cast<int>(lockState);
			return;
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

void DoorModule::getCP()
{
	for (const auto &obj : (*cache_))
	{
		if (obj->name == "CP")
		{
			cpObj_ = static_cast<CP*>(obj);
			return;
		}
	}
	BOOST_LOG(logger_) << "ERROR " << "DoorModule::getCP: There is no CP in cache";
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
	doorsObj_->commonLockGND->connectors[0]->value = 1;
	RESULT* result = new RESULT();
	result->status = RESULT::EStatus::success;
	result->applicant = "BDM_DOOR";
	result->feedback = std::to_string(doorsObj_->commonLockGND->connectors[0]->id) + std::to_string(doorsObj_->commonLockGND->connectors[0]->value);
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
			if (cpObj_->autoClosingWindow && door->window->opened)
			{
				BOOST_LOG(logger_) << "INFO " << "DoorModule::lockDoors auto-closing window: " << door->label;
				door->window->close();
			}
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

void DoorModule::openWindow(std::string port)
{
	if (doorsObj_->container_.size() == 6)
	{
		//displayTopology();
		for (auto &door : doorsObj_->container_)
		{

			if (door->label == door6Labels_[std::stoi(port) - 6])
			{
				BOOST_LOG(logger_) << "INFO " << "DoorModule::openWindow: " << door->label;
				door->window->open();
				break;
			}

		}
	}
}

void DoorModule::closeWindow(std::string port)
{
	if (doorsObj_->container_.at(0)->window->lockingState != WINDOW::ELockingState::locked)
	{
		if (doorsObj_->container_.size() == 6)
		{
			//displayTopology();
			for (auto &door : doorsObj_->container_)
			{

				if (door->label == door6Labels_[std::stoi(port) - 6])
				{
					BOOST_LOG(logger_) << "INFO " << "DoorModule::closeWindow: " << door->label;
					door->window->close();
					break;
				}
			}
		}
	}
	else
	{
		BOOST_LOG(logger_) << "INFO " << "DoorModule::closeWindow: cannot close window because is locked";
	}
	
}

void DoorModule::lockWindow()
{
	BOOST_LOG(logger_) << "INFO " << "DoorModule::lockWindow";
	for (auto &door : doorsObj_->container_)
	{
		door->window->lockingState = WINDOW::ELockingState::locked;
	}
}

void DoorModule::unlockWindow()
{
	BOOST_LOG(logger_) << "INFO " << "DoorModule::unlockWindow";
	for (auto &door : doorsObj_->container_)
	{
		door->window->lockingState = WINDOW::ELockingState::unlocked;
	}
}

void DoorModule::changeConnectorState(std::string connectorId, std::string value)
{
	BOOST_LOG(logger_) << "INF " << "DoorModule::changeConnectorState";
	for (const auto &door : doorsObj_->container_)
	{
		for (const auto &port : door->ports)
		{
			for ( auto &conn : port->connectors)
			{
				if (conn->id == std::stoi(connectorId))
				{
					BOOST_LOG(logger_) << "INF " << "DoorModule::changeConnectorState: on door: " << door->label
						<< " on port" << port->label << " connector ID: " << conn->id << " to value " << std::stoi(value);
					door->changeConnectorState(std::stoi(connectorId), std::stoi(value));
					changeDOORSOpeningStateIfNeeded(std::stoi(value));
					return;
				}
					

			}
		}
	}
	for (auto &conn : doorsObj_->commonLockGND->connectors)
	{
		if (conn->id == std::stoi(connectorId))
		{
			BOOST_LOG(logger_) << "INF " << "DoorModule::changeConnectorState: GND connector " << conn->id;
			doorsObj_->setLockingState(std::stoi(value));

		}
	}
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
		doorsObj_->openingState = static_cast<DOORS::EOpeningState>(value);

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
