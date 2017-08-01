#include "stdafx.h"
#include "ControlsManager.hpp"


ControlsManager::ControlsManager(Cache* cachePtr, boost::log::sources::logger_mt logger) : cachePtr_(cachePtr),
	logger_(logger), file_("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\UIA_dump.txt", std::ios::app)
{
	file_ << "";
	doors_ = DOORS();
	
}


ControlsManager::~ControlsManager()
{
	file_.close();
}

void ControlsManager::initialize()
{
	doorsSubsIds_ = cachePtr_->subscribe("DOORS", std::bind(&ControlsManager::handleDoorsChange, this, std::placeholders::_1), {0, 1 });
	lightsSubsIds_ = cachePtr_->subscribe("LIGHT", std::bind(&ControlsManager::handleLightChange, this, std::placeholders::_1), {0, 1 });
}

void ControlsManager::handleDoorsChange(Obj* obj)
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	auto doors = static_cast<DOORS*>(obj);
	if (doors_.children.empty())
	{
		doors_ = *doors;
	}
	else
	{
		if (doors_.lockingState != doors->lockingState)
		{
			doors_ = *doors;
			if (doors_.lockingState == DOORS::ELockingState::unlocked)
				file_ << "DOORS_UNLOCKED" << std::endl;
			else
				file_ << "DOORS_LOCKED" << std::endl;
		}
		else if (doors_.openingState != doors->openingState)
		{
			doors_ = *doors;
			if (doors_.openingState == DOORS::EOpeningState::opened)
				file_ << "DOORS_OPENED" << std::endl;
			else
				file_ << "DOORS_CLOSED" << std::endl;
		}
	}
}

void ControlsManager::handleLightChange(Obj* obj)
{
	
}

