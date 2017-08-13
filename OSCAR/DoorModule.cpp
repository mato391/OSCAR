#include "stdafx.h"
#include "DoorModule.hpp"


DoorModule::DoorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr)
{
	logger_ = logger;
	cachePtr_ = cachePtr;
	BOOST_LOG(logger_) << "DEBUG " << "DoorModule ctor";
}


DoorModule::~DoorModule()
{
}

void DoorModule::initialize()
{
	BOOST_LOG(logger_) << "INFO " << "DoorModule::initialize";
	getBDMModules();
	prepareTopology();
	RESULT* res = new RESULT();
	res->applicant = "DOOR_MODULE";
	res->feedback = "Initialize";
	res->status = RESULT::EStatus::success;
	bdmModuleObj_->children.push_back(res);
}

void DoorModule::setup()
{
	BOOST_LOG(logger_) << "DBG " << "DoorModule::setup";
	bdmModuleObj_->protocol = MODULE::EProtocol::CSimpleMessage;
	bdmModuleObj_->operationalState = MODULE::EOperationalState::enabled;
	ccIndSubscrId_ = cachePtr_->subscribe("CHANGE_CONNECTOR_DONE_IND", std::bind(&DoorModule::changeConnectorIndHandler, this, std::placeholders::_1), { 0 })[0];
	
}

void DoorModule::changeConnectorIndHandler(Obj* obj)
{
	auto ccsi = static_cast<CHANGE_CONNECTOR_DONE_IND*>(obj);
	if (ccsi->domain == bdmModuleObj_->domain)
	{
		int connId = ccsi->connector->id;
		int connValue = ccsi->connector->value;
		BOOST_LOG(logger_) << "INF " __FUNCTION__ << " connId " << connId << " connValue " << connValue;
		if (ccsi->connector->type == CONNECTOR::EType::input)
		{
			auto doorC = getDoorByRefId(connId);
			if (doorC == nullptr)
				return;
			doorC->openingState = static_cast<DOOR::EOpeningState>(connValue);
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " door " << doorC->label << " has been "
				<< ((doorC->openingState == DOOR::EOpeningState::closed) ? "closed" : "opened");
			//checking if all doors are closed and changing state of doorsObj should be added
		}
		else
		{
			doorsObj_->setLockingState(connValue);
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " DOORS " << ((doorsObj_->lockingState == DOORS::ELockingState::locked) ? " locked " : " unlocked ");
			cachePtr_->commitChanges(doorsObj_);
		}
	}
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
	doorsObj_->commonLockGND = new CONNECTOR(-1);
	std::vector<CONNECTOR*> connectors;
	auto conns = cachePtr_->getAllObjectsUnder(bdmModuleObj_, "CONNECTOR");
	BOOST_LOG(logger_) << "DBG " << "DoorModule::prepareTopology " << bdmModuleObj_->label;
	BOOST_LOG(logger_) << "DBG " << "DoorModule::prepareTopology " << conns.size();
	std::vector<std::string> labels;
	for (const auto &obj : conns)
	{

		auto exist = [](std::string label, std::vector<std::string>* labelVec)->bool
		{
			for (const auto &lab : *labelVec)
			{
				if (lab == label)
					return true;
			}
			return false;

		}(static_cast<CONNECTOR*>(obj)->label, &labels);
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " is exist " << exist;
		if (!exist 
			&& static_cast<CONNECTOR*>(obj)->label.find("WINDOW") == std::string::npos
			&& static_cast<CONNECTOR*>(obj)->label.find("GND") == std::string::npos)
		{
			labels.push_back(static_cast<CONNECTOR*>(obj)->label);
		}
		else if (static_cast<CONNECTOR*>(obj)->label.find("GND") != std::string::npos)
		{
			doorsObj_->commonLockGND = static_cast<CONNECTOR*>(obj);
		}
	}
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " count of labels: " << labels.size();
	for (const auto &label : labels)
	{
		auto door = new DOOR(label);
		for (const auto &obj : conns)
		{
			auto connC = static_cast<CONNECTOR*>(obj);
			BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connector " << connC->label
				<< " connectorType " << static_cast<int>(connC->type);
			auto exist = [](DOOR* door, CONNECTOR* conn)->bool
			{
				if (door->label == conn->label)
				{
					for (const auto &ref : door->refs)
					{
						if (ref == conn->id)
							return true;
					}
					return false;
				}
				else if (conn->label.find("GND") != std::string::npos)
					return false;
				
				return true;
			}(door, connC);
			BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " exist? " << exist;
			if (!exist)
			{
				door->refs.push_back(connC->id);
			}
		}
		cachePtr_->addToChildren(doorsObj_, door);
	}
	
	cachePtr_->addToChildren(bdmModuleObj_, doorsObj_);
	
	
	
	
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " count of labels: " << doorsObj_->children.size();
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " common: " << doorsObj_->commonLockGND->id;
	for (const auto &obj : doorsObj_->children)
	{
		auto door = static_cast<DOOR*>(obj);
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " door " << door->label;
		for (const auto &ref : door->refs)
			BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " REF " << ref;
	}
}

DOOR* DoorModule::getDoorByLabel(std::string label)
{
	for (const auto &door : doorsObj_->children)
	{
		auto doorC = static_cast<DOOR*>(door);
		if (doorC->label == label)
			return doorC;
	}
	return nullptr;
}

DOOR* DoorModule::getDoorByRefId(int ref)
{
	for (const auto &door : doorsObj_->children)
	{
		auto doorC = static_cast<DOOR*>(door);
		for (const auto &refId : doorC->refs)
		{
			if (refId == ref)
			{
				return doorC;
			}
		}
	}
}

void DoorModule::unlockDoors()
{
	
}

void DoorModule::lockDoors()
{
	
}

