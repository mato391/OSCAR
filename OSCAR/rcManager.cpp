#include "stdafx.h"
#include "rcManager.hpp"


rcManager::rcManager(MODULE* wcmModule, boost::log::sources::logger_mt logger) : 
	wcmModule_(wcmModule), 
	logger_(logger)
{
}


rcManager::~rcManager()
{
}

void rcManager::prepareRCInterface()
{
	BOOST_LOG(logger_) << "INF " << "rcManager::prepareRCInterface";
	for (auto &connVec : wcmModule_->connectors_)
	{
		for (auto &conn : connVec)
		{
			if (static_cast<ANTENNA*>(conn)->label == "RC_SIMPLE")
			{
				BOOST_LOG(logger_) << "INF " << "rcManager::prepareRCInterface " << "SIMPLE RC has been found";
				rControler_ = new REMOTE_CONTROLLER("");
				wcmModule_->children.push_back(rControler_);
				return;
			}
			else
			{
				configureAdvancedRemoteController();
			}
		}
	}
}

void rcManager::configureAdvancedRemoteController()
{
	BOOST_LOG(logger_) << "INF " << "rcManager::configureAdvancedRemoteController " << "Advanced remote controller configuration";
}
