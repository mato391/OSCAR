#include "stdafx.h"
#include "DoorModule.hpp"


DoorModule::DoorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr)
{
	logger_ = logger;
	cache_ = cache;
	cachePtr_ = cachePtr;
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
	moduleTaskSubscrId_ = cachePtr_->subscribe("MODULE_TASK", std::bind(&DoorModule::checkAndExecuteTask, this, std::placeholders::_1), { 0 })[0];	//subscribe for create
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " subscribed for MODULE_TASK. SubsId: " << moduleTaskSubscrId_;
	if (wrongSetupConnectors.size() == 0)
	{
		//should be result with status OK
	}
	else
	{
		//should be result with partial OK and feedback
	}
}

void DoorModule::checkAndExecuteTask(Obj* obj)
{
	BOOST_LOG(logger_) << "VIP " << "DoorModule::checkAndExecuteTask thread " << boost::this_thread::get_id();
	runTask(static_cast<MODULE_TASK*>(obj));
	cachePtr_->removeFromChild(bdmModuleObj_, obj);
}

void DoorModule::runTask(MODULE_TASK* task)
{
	if (task->type == MODULE_TASK::EName::CHANGE_CONNECTOR_STATE_TASK && task->taskFor == bdmModuleObj_->domain)
	{
		auto ccst = static_cast<CHANGE_CONNECTOR_STATE_TASK*>(task);
		auto feedback = changeConnectorState(ccst->port, ccst->value);

		/*if (feedback != boost::none)
		{
			auto result = new RESULT();
			result->applicant = "DoorModule";
			result->feedback = feedback.value();
			result->status = RESULT::EStatus::success;
			cachePtr_->addToChildren(task, result);
			BOOST_LOG(logger_) << "INF " << "DoorModule::runTask: feedback: " << result->feedback;
		}*/
	}
	else
		BOOST_LOG(logger_) << "INF " << "DoorModule::runTask " << "This task is not for DoorModule or type is unknown " << task->taskFor << " != " << bdmModuleObj_->domain;
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
	auto moduleVec = cachePtr_->getAllObjectsFromChildren("EQM", "MODULE");
	if (!moduleVec.empty())
	{
		BOOST_LOG(logger_) << "INF " << "DoorModule::getBDMModules " << "EQM object found";
		for (const auto &mod : moduleVec)
		{
			if (static_cast<MODULE*>(mod)->label.find("BDM_DOOR") != std::string::npos)
			{
				BOOST_LOG(logger_) << "INF " << "DoorModule::getBDMModule: MODULE found";
				bdmModuleObj_ = static_cast<MODULE*>(mod);
				return;
			}
		}
	}
	BOOST_LOG(logger_) << "ERR " << "DoorModule::getBDMModule: MODULE not found";
}


void DoorModule::prepareTopology()
{
	doorsObj_ = new DOORS();
	doorsObj_->commonLockGND = new PORT();
	std::vector<CONNECTOR*> connectors;
	BOOST_LOG(logger_) << "DBG " << "DoorModule::prepareTopology" << bdmModuleObj_->label;
	BOOST_LOG(logger_) << "DBG " << "DoorModule::prepareTopology" << bdmModuleObj_->children.size();
	BOOST_LOG(logger_) << "DBG " << "DoorModule::prepareTopology" << bdmModuleObj_->connectors_.size();
	for (const auto &obj : bdmModuleObj_->children)
	{
		BOOST_LOG(logger_) << "DBG DoorModule::prepareTopology " << obj->name;
		if (obj->name == "CONNECTOR")
		{
			if (static_cast<CONNECTOR*>(obj)->label.find("DOOR") != std::string::npos)
			{
				connectors.push_back(static_cast<CONNECTOR*>(obj));
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
	cachePtr_->addObject(doorsObj_);
	//cache_->push_back(doorsObj_);
}

void DoorModule::createDoors(std::vector<CONNECTOR*> connectors)
{
	BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors";
	std::vector<PORT*> ports;
	CONNECTOR* commonLockGNDConn = new CONNECTOR(-1);
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
			port->children.push_back(conn);
			ports.push_back(port);
		}
		else
		{
			BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors: port label exist. Creating : " << label;
			for (const auto &port : ports)
			{
				if (port->label == label)
				{
					port->children.push_back(conn);
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
			door->children.push_back(port);
			cachePtr_->addToChildren(doorsObj_, door);
		}
		else if (port->label.find("GND") != std::string::npos)
		{
			doorsObj_->commonLockGND = port;
			if (commonLockGNDConn->id != -1)
			{
				doorsObj_->commonLockGND->children.push_back(commonLockGNDConn);
				cachePtr_->commitChanges(doorsObj_);
			}
		}
		else
		{
			BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors: port label does exist.";
			for (const auto &door : doorsObj_->children)
			{
				if (door->name == "DOOR")
				{
					if (static_cast<DOOR*>(door)->label == port->label)
					{
						door->children.push_back(port);
						break;
					}
				}
			}
		}
	}
	BOOST_LOG(logger_) << "INF " << "DoorModule::createDoors: " << doorsObj_->children.size() << " door has been created";
}

bool DoorModule::checkDoesDoorExist(std::string label)
{
	for (const auto &door : doorsObj_->children)
	{
		if (door->name == "DOOR")
		{
			if (static_cast<DOOR*>(door)->label == label)
				return true;
		}
	}
	return false;
}

void DoorModule::unlockDoors()
{
	BOOST_LOG(logger_) << "INFO " << "DoorModule::unlockDoors";
	for (auto &obj : doorsObj_->children)
	{
		if (obj->name == "DOOR")
		{
			auto door = static_cast<DOOR*>(obj);
			BOOST_LOG(logger_) << "INFO " << "DoorModule::unlockDoors " << door->label;
			door->unlockDoor();
		}
		
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
		for (auto &obj : doorsObj_->children)
		{
			if (obj->name == "DOOR")
			{
				auto door = static_cast<DOOR*>(obj);
				BOOST_LOG(logger_) << "INFO " << "DoorModule::lockDoors " << door->label;
				door->lockDoor();
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
	for (const auto &obj : doorsObj_->children)
	{
		if (obj->name == "DOOR")
		{
			auto door = static_cast<DOOR*>(obj);
			if (door->openingState == DOOR::EOpeningState::opened)
			{
				return new std::string(door->label);
			}
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
	for (const auto &obj : doorsObj_->children)		//refactor 3 fory?!
	{
		auto door = static_cast<DOOR*>(obj);
		for (const auto &port : door->children)
		{
			auto portC = static_cast<PORT*>(port);
			for ( auto &conn : port->children)
			{
				auto connC = static_cast<CONNECTOR*>(conn);
				if (connC->id == connectorId)
				{
					BOOST_LOG(logger_) << "INF " << "DoorModule::changeConnectorState: on door: " << door->label
						<< " on port" << portC->label << " connector ID: " << connC->id << " to value " << value;
					door->changeConnectorState(connectorId, value);
					cachePtr_->commitChanges(door);
					changeDOORSOpeningStateIfNeeded(value);
					return boost::none;
				}
			}
		}
	}
	for (auto &conn : doorsObj_->commonLockGND->children)
	{
		auto connC = static_cast<CONNECTOR*>(conn);
		if (connC->id == connectorId)
		{
			BOOST_LOG(logger_) << "INF " << "DoorModule::changeConnectorState: GND connector " << connC->id << " to value " << value;
			doorsObj_->setLockingState(value);
			cachePtr_->commitChanges(doorsObj_);
			boost::thread t(std::bind(&DoorModule::setTimerForClose, this));
			t.detach();
			return (value == 0) ? "LOCK_DOORS" : "UNLOCK_DOORS";
		}
	}
	BOOST_LOG(logger_) << "ERR " << "DoorModule::changeConnectorState: Connector not found...";
	return boost::none;
}

void DoorModule::setTimerForClose()
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	if (doorsObj_->lockingState == DOORS::ELockingState::unlocked)
	{
		int i = 0;
		while (i < 10)
		{
			if (doorsObj_->openingState == DOORS::EOpeningState::opened)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " doors opened. ShutDown timer";
				return;
			}
			boost::this_thread::sleep(boost::posix_time::seconds(1));
			i++;
			std::cout << "TIMER " << i << std::endl;
		}
		std::cout << "TIMER CLOSE" << std::endl;
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " timeout for opening doors";
		RESULT* res = new RESULT();
		res->applicant = "DOOR_MODULE";
		res->feedback = std::to_string(static_cast<CONNECTOR*>(doorsObj_->commonLockGND->children[0])->id) + ":0";
		res->status = RESULT::EStatus::success;
		res->type = RESULT::EType::executive;
		cachePtr_->addToChildren(bdmModuleObj_, res);

	}
}

void DoorModule::changeDOORSOpeningStateIfNeeded(int value)
{
	bool diff = false;
	for (const auto &door : doorsObj_->children)
	{
		if (static_cast<DOOR*>(door)->openingState != static_cast<DOOR::EOpeningState>(value))
			diff = true;
	}
	if (!diff && doorsObj_->openingState != static_cast<DOORS::EOpeningState>(value))
	{
		doorsObj_->openingState = static_cast<DOORS::EOpeningState>(value);
		cachePtr_->commitChanges(doorsObj_);
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
	if (doorsObj_->commonLockGND->children.size() > 0)
	{
		BOOST_LOG(logger_) << "DEBUG " << "CONNECTOR " << static_cast<CONNECTOR*>(doorsObj_->commonLockGND->children[0])->id << " "
			<< static_cast<CONNECTOR*>(doorsObj_->commonLockGND->children[0])->label;
	}
	for (const auto &door : doorsObj_->children)
	{
		if (door->name == "DOOR")
		{
			auto doorC = static_cast<DOOR*>(door);
			BOOST_LOG(logger_) << "DEBUG " << doorC->label;
			for (const auto &port : door->children)
			{
				if (port->name == "PORT")
				{
					auto portC = static_cast<PORT*>(port);
					BOOST_LOG(logger_) << "DEBUG " << "PORT " << portC->label;
					for (const auto &conn : port->children)
					{
						if (conn->name == "CONNECTOR")
						{
							auto connC = static_cast<CONNECTOR*>(conn);
							BOOST_LOG(logger_) << "DEBUG " << "CONNECTOR " << connC->id << " " << static_cast<int>(connC->type);
						}
					}
				}
			}
		}
	}
}
