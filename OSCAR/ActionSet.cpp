#include "stdafx.h"
#include "ActionSet.hpp"


ActionSet::ActionSet(boost::log::sources::logger_mt logger)
{
	logger_ = logger;
}


ActionSet::~ActionSet()
{
}

Action* ActionSet::getAction(std::string name)
{
	for (const auto &action : actions_)
	{
		if (action->name == name)
		{
			BOOST_LOG(logger_) << "INF " __FUNCTION__ << " action name: " << name;
			return action;
		}
			
	}
	return nullptr;
}

void ActionSet::addAction(std::vector<int> connsIds, std::vector<int> connValues, std::string name)
{
	actions_.push_back(new Action(connsIds, connValues, name));
}
