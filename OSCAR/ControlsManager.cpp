#include "stdafx.h"
#include "ControlsManager.hpp"


ControlsManager::ControlsManager(Cache* cachePtr, boost::log::sources::logger_mt logger) : cachePtr_(cachePtr),
	logger_(logger), file_("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\UIA_dump.txt", std::ios::app)
{
	std::fstream file("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\UIA_dump.txt", std::ios::out);
	file << "";
	file.close();
	doors_ = DOORS();
	
}


ControlsManager::~ControlsManager()
{
	file_.close();
}

void ControlsManager::initialize()
{
	doorsSubsIds_ = cachePtr_->subscribe("DOORS", std::bind(&ControlsManager::handleDoorsChange, this, std::placeholders::_1), {0, 1 });
	lightesSubsId_ = cachePtr_->subscribe("LIGHTES", std::bind(&ControlsManager::handleLightesCreation, this, std::placeholders::_1), { 0 })[0];
	lightsSubsIds_ = cachePtr_->subscribe("LIGHT", std::bind(&ControlsManager::handleLightChange, this, std::placeholders::_1), {0, 1});
}

void ControlsManager::handleLightesCreation(Obj* obj)
{
	lightes_ = *(static_cast<LIGHTES*>(obj));
	cachePtr_->unsubscribe(lightesSubsId_, 0);
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
	auto newLight = static_cast<LIGHT*>(obj);
	for (auto &light : lightes_.children)
	{
		if (light->name == "LIGHT" && static_cast<LIGHT*>(light)->label == newLight->label)
		{
			auto lightC = *static_cast<LIGHT*>(light);
			if (lightC.proceduralState != newLight->proceduralState)
			{
				lightC.proceduralState = newLight->proceduralState;
				if (lightC.proceduralState == LIGHT::EProceduralState::off)
					file_ << lightC.label << ":OFF" << std::endl;
				else
					file_ << lightC.label << ":ON" << std::endl;
			}
				
		}
	}

}

