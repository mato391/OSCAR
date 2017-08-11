#include "stdafx.h"
#include "ConnectorManager.hpp"


ConnectorManager::ConnectorManager(boost::log::sources::logger_mt logger, Cache* cachePtr) : logger_(logger), cachePtr_(cachePtr)
{
}

void ConnectorManager::getModules()
{
	eqm_ = cachePtr_->getUniqueObject("EQM");
	if (eqm_ != nullptr)
	{
		modules_ = cachePtr_->getAllObjectsUnder(eqm_, "MODULE");
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MODULES: " << modules_.size();
	}
	else
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " no eqm";
	
}

ConnectorManager::~ConnectorManager()
{
}

void ConnectorManager::handleMaskConnectorChange(CMESSAGE::CMessage* msg)
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	auto cMaskMessage = static_cast<CMESSAGE::CMaskMessage*>(msg);
	std::string domain = msg->fromDomain;
	std::vector<std::bitset<8>> masks = { std::bitset<8>(cMaskMessage->mask1), std::bitset<8>(cMaskMessage->mask2), 
		std::bitset<8>(cMaskMessage->mask3), std::bitset<8>(cMaskMessage->mask4) };
	for (const auto &mask : masks)
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " " << mask.to_string();

	auto moduleConnectors = getConnectorsFromModule(domain);
	if (!moduleConnectors.empty())
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " connectors found";
		maskConnectors(moduleConnectors, masks);
		for (const auto &module : modules_)
		{
			if (static_cast<MODULE*>(module)->domain == domain)
			{
				CONNECTORS_MASKING_DONE_IND* cmdi = new CONNECTORS_MASKING_DONE_IND();
				cmdi->connectors_ = changed_;
				cmdi->domain = domain;
				cachePtr_->addToChildren(module, cmdi);
			}
		}
	}
	else
		BOOST_LOG(logger_) << "WRN " << __FUNCTION__ << " No connectors";
		
}

void ConnectorManager::handleSingleConnectorChange(CMESSAGE::CMessage* msg)
{
	
}

std::vector<Obj*> ConnectorManager::getConnectorsFromModule(std::string domain)
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " domain: " << domain;
	for (const auto &module : modules_)
	{
		auto moduleC = static_cast<MODULE*>(module);
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " modules domain: " << moduleC->domain;
		if (moduleC->domain == domain)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " returning connectors";
			return cachePtr_->getAllObjectsUnder(moduleC, "CONNECTOR");
		}
	}
	BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " no module with domain " << domain;
	return {};
}

void ConnectorManager::maskConnectors(std::vector<Obj*> connectors, std::vector<std::bitset<8>> masks)
{
	int connsCount = connectors.size();
	if (!changed_.empty())
		changed_.clear();
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connsCount: " << connsCount;
	if (connsCount > 0 && masks[0] != 0)
	{
		for (int i = 0; i < 8; i++)
		{
			if (masks[0][7 - i] == 1 && i < connsCount)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " mask[0][" << i << "] its 1 changing connector " << static_cast<CONNECTOR*>(connectors[i])->label;
				static_cast<CONNECTOR*>(connectors[i])->value = !static_cast<CONNECTOR*>(connectors[i])->value;
				changed_.push_back(static_cast<CONNECTOR*>(connectors[i]));
			}
		}
	}
	if (connsCount > 8 && masks[1] != 0)
	{
		for (int i = 8; i < 16; i++)
		{
			if (masks[1][7 - (i - 8)] == 1 && i < connsCount)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " mask[1][" << i << "] its 1 changing connector " << static_cast<CONNECTOR*>(connectors[i])->label;
				static_cast<CONNECTOR*>(connectors[i])->value = !static_cast<CONNECTOR*>(connectors[i])->value;
				changed_.push_back(static_cast<CONNECTOR*>(connectors[i]));
			}
		}
	}
	if (connsCount > 16 && masks[2] != 0)
	{
		for (int i = 16; i < 24; i++)
		{
			if (masks[2][7 - (i - 16)] == 1 && i < connsCount)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " mask[2][" << i << "] its 1 changing connector " << static_cast<CONNECTOR*>(connectors[i])->label;
				static_cast<CONNECTOR*>(connectors[i])->value = !static_cast<CONNECTOR*>(connectors[i])->value;
				changed_.push_back(static_cast<CONNECTOR*>(connectors[i]));
			}
		}
	}
	if (connsCount > 24 && masks[3] != 0)
	{
		for (int i = 8; i < connsCount; i++)
		{
			if (masks[3][7 - (i - 24)] == 1 && i < connsCount)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " mask[3][" << i << "] its 1 changing connector " << static_cast<CONNECTOR*>(connectors[i])->label;
				static_cast<CONNECTOR*>(connectors[i])->value = !static_cast<CONNECTOR*>(connectors[i])->value;
				changed_.push_back(static_cast<CONNECTOR*>(connectors[i]));
			}
		}
	}
}
