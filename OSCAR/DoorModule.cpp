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
	prepareTopology();
	displayTopology();
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

void DoorModule::prepareTopology()
{
	doorsObj_ = new DOORS();
	std::vector<std::string> labels;
	if (cpObj_->doorsVersion == 6)
		labels = door6Labels_;
	else if (cpObj_->doorsVersion == 5)
		labels = door5Labels_;
	else if (cpObj_->doorsVersion == 4)
		labels = door4Labels_;
	BOOST_LOG(logger_) << "INFO " << "DoorModule::prepareTopology for: " << labels.size() << " door";
	for (const auto &label : labels)
	{
		DOOR* door = new DOOR(label);
		door->lockDoor();
		door->closeDoor();
		doorsObj_->addDoors(door);
	}
	cache_->push_back(doorsObj_);
}

void DoorModule::unlockDoors()
{
	BOOST_LOG(logger_) << "INFO " << "DoorModule::unlockDoors";
	for (auto &door : doorsObj_->container_)
	{
		BOOST_LOG(logger_) << "INFO " << "DoorModule::unlockDoors " << door->label;
		door->unlockDoor();
	}
}

void DoorModule::lockDoors()
{
	BOOST_LOG(logger_) << "INFO " << "DoorModule::lockDoors";
	std::string* label = checkIfDoorsAreClosed();
	if (label == nullptr && !checkIfBateryAlarmRaised())
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

void DoorModule::changeOpeningState(std::string port, DOOR::EOpeningState state)
{
	
	if (doorsObj_->container_.size() == 6)
	{
		//displayTopology();
		for (auto &door : doorsObj_->container_)
		{
			
			if (door->label == door6Labels_[std::stoi(port)])
			{
				BOOST_LOG(logger_) << "INFO " << "DoorModule::changeOpeningState: " << static_cast<int>(state) << " on " << door->label;
				if (state == DOOR::EOpeningState::opened)
					door->openDoor();
				else
					door->closeDoor();
				break;
			}
			
		}
	}
}

void DoorModule::displayTopology()
{
	BOOST_LOG(logger_) << "DEBUG " << "DoorModule::displayTopology";
	for (const auto &door : doorsObj_->container_)
	{
		BOOST_LOG(logger_) << "DEBUG " << door->label;
	}
}
