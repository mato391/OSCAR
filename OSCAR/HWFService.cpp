#include "stdafx.h"
#include "HWFService.hpp"


HWFService::HWFService(EQM* eqmObjPtr, boost::log::sources::logger_mt logger, Cache* cachePtr)
	: eqmObjPtr_(eqmObjPtr),
	logger_(logger),
	cachePtr_(cachePtr)
{
	BOOST_LOG(logger) << "INF " << "HWFService ctor";
	loadHwf();
}


HWFService::~HWFService()
{
}

void HWFService::loadHwf()
{
	BOOST_LOG(logger_) << "INF " << "HWFService::loadHwf: from: " << hwfPath_;
	std::fstream hwfFile(hwfPath_, std::ios::in);
	hwfFile >> hwfFileContent_;
	hwfFile.close();
}

void HWFService::prepareTopology()
{
	BOOST_LOG(logger_) << "INF " << "HWFService::prepareTopology";
	std::vector<std::string> moduleRawData;
	std::vector<Obj*> modules;
	boost::split(moduleRawData, hwfFileContent_, boost::is_any_of(";"));
	BOOST_LOG(logger_) << "INF " << "HWFService::prepareTopology: modulesSplitted: "
		<< moduleRawData.size();
	for (auto &module : moduleRawData)
	{
		if (module != "")
		{
			BOOST_LOG(logger_) << "DBG " << "HWFService::prepareTopology: module: " << module;
			auto moduleObj = createModule(module);
			createModuleTopology(moduleObj, module);
			modules.push_back(moduleObj);
		}
		
	}
	for (const auto &module : modules)
	{
		eqmObjPtr_->children.push_back(module);
	}
}

MODULE* HWFService::createModule(std::string data)
{
	//BOOST_LOG(logger_) << "DBG " << "HWFService::createModule: " << data;
	std::vector<std::string> elementRawData;
	boost::split(elementRawData, data, boost::is_any_of("|"));
	std::vector<std::string> moduleInfRawData;
	boost::split(moduleInfRawData, elementRawData[0], boost::is_any_of(":"));
	std::vector<std::string> moduleInf;
	//BOOST_LOG(logger_) << "DBG " << "HWFService::createModule: moduleInf.size: " << moduleInf.size();
	boost::split(moduleInf, moduleInfRawData[1], boost::is_any_of(","));
	MODULE* module = new MODULE();
	module->serialNumber = moduleInf[0];
	module->domain = moduleInf[1];
	module->label = moduleInf[2];
	BOOST_LOG(logger_) << "DBG " << "HWFService::createModule: module->label:" << module->label;
	module->detectionStatus = MODULE::EDetectionStatus::offline;
	return module;
}

void HWFService::createModuleTopology(MODULE* moduleObj, std::string data)
{
	std::vector<std::string> elementRawData;
	std::vector<Obj*> connectorsGroup;
	boost::split(elementRawData, data, boost::is_any_of("|"));
	for (std::vector<std::string>::iterator i = elementRawData.begin() + 1; i < elementRawData.end(); i++)
	{
		if ((*i).find("Connector:") != std::string::npos)
		{
			BOOST_LOG(logger_) << "DBG " << " HWFService::createModuleTopology in for: " << *i;
			std::vector<std::string> connectorRawData;
			boost::split(connectorRawData, *i, boost::is_any_of(":"));
			std::vector<std::string> connectorData;
			boost::split(connectorData, connectorRawData[1], boost::is_any_of(","));
			//BOOST_LOG(logger_) << "DBG " << " HWFService::createModuleTopology connectorData.size: " << connectorData.size();
			CONNECTOR* conn = new CONNECTOR(std::stoi(connectorData[0]));
			conn->label = connectorData[1];
			conn->used = true;
			conn->type = static_cast<CONNECTOR::EType>(std::stoi(connectorData[2]));
			cachePtr_->addToChildren(moduleObj, conn);	//NEW CACHE
			BOOST_LOG(logger_) << "DBG " __FUNCTION__ << " MODULE: " << moduleObj->label << " created CONNECTOR: " << conn->label << " type: " << static_cast<int>(conn->type);
			
		}
		else if ((*i).find("Antenna") != std::string::npos)
		{
			//BOOST_LOG(logger_) << "DBG " << " HWFService::createModuleTopology in for: " << *i;
			std::vector<std::string> connectorRawData;
			boost::split(connectorRawData, *i, boost::is_any_of(":"));
			std::vector<std::string> connectorData;
			boost::split(connectorData, connectorRawData[1], boost::is_any_of(","));
			//BOOST_LOG(logger_) << "DBG " << " HWFService::createModuleTopology AntennaData.size: " << connectorData.size();
			ANTENNA* antenna = new ANTENNA();
			antenna->id = std::stoi(connectorData[0]);
			antenna->label = connectorData[1];
			cachePtr_->addToChildren(moduleObj, antenna);
		}
		else if ((*i).find("Switch") != std::string::npos)
		{
			BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " creating switch";
			std::vector<std::string> connectorRawData;
			boost::split(connectorRawData, *i, boost::is_any_of(":"));
			std::vector<std::string> connectorData;
			boost::split(connectorData, connectorRawData[1], boost::is_any_of(","));
			//BOOST_LOG(logger_) << "DBG " << " HWFService::createModuleTopology AntennaData.size: " << connectorData.size();
			SWITCH_CONNECTOR* sConn = new SWITCH_CONNECTOR();
			sConn->id = std::stoi(connectorData[0]);
			sConn->label = connectorData[1];
			cachePtr_->addToChildren(moduleObj, sConn);
		}
	}
	
}


