#pragma once

#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

#include "Cache.hpp"
#include "CMessage.hpp"

#include <string>
#include <vector>

#include "CONNECTOR.hpp"
#include "CONNECTORS_MASKING_DONE_IND.hpp"
#include "CHANGE_CONNECTOR_DONE_IND.hpp"
#include "SWITCH_CONNECTOR.hpp"


class ConnectorManager
{
public:
	ConnectorManager(boost::log::sources::logger_mt logger, Cache* cachePtr);
	~ConnectorManager();
	void handleSingleConnectorChange(CMESSAGE::CMessage* msg);
	void handleMaskConnectorChange(CMESSAGE::CMessage* msg);
	void getModules();
private:
	boost::log::sources::logger_mt logger_;
	Cache* cachePtr_;
	Obj* eqm_;
	std::vector<Obj*> modules_;
	std::vector<Obj*> changed_;

	std::vector<Obj*> getConnectorsFromModule(std::string domain);
	void maskConnectors(std::vector<Obj*> connectors, std::vector<std::bitset<8>> masks, bool initial);
};

