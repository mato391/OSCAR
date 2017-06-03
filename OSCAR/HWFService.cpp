#include "stdafx.h"
#include "HWFService.hpp"


HWFService::HWFService(EQM* eqmObjPtr) : eqmObjPtr_(eqmObjPtr)
{
	loadHwf();
}


HWFService::~HWFService()
{
}

void HWFService::loadHwf()
{
	std::fstream hwfFile(hwfPath_, std::ios::in);
	hwfFile >> hwfFileContent_;
	hwfFile.close();
}

void HWFService::prepareTopology()
{
	
	std::vector<std::string> moduleRawData;
	std::vector<Obj*> modules;
	boost::split(moduleRawData, hwfFileContent_, boost::is_any_of(";"));
	std::cout << "HWFService::prepareTopology: modulesSplitted: " << moduleRawData.size() << std::endl;
	for (auto &module : moduleRawData)
	{
		if (module != "")
		{
			std::cout << "prepareTopology: " << module << std::endl;
			auto moduleObj = createModule(module);
			createModuleTopology(moduleObj, module);
			modules.push_back(moduleObj);
		}
		
	}
	eqmObjPtr_->modules_ = modules;
}

MODULE* HWFService::createModule(std::string data)
{
	std::cout << "HWFService::createModule: " << data << std::endl;
	std::vector<std::string> elementRawData;
	boost::split(elementRawData, data, boost::is_any_of("|"));
	std::vector<std::string> moduleInfRawData;
	boost::split(moduleInfRawData, elementRawData[0], boost::is_any_of(":"));
	std::vector<std::string> moduleInf;
	std::cout << "HWFService::createModule: " << moduleInf.size() << std::endl;
	boost::split(moduleInf, moduleInfRawData[1], boost::is_any_of(","));
	MODULE* module = new MODULE();
	module->serialNumber = moduleInf[0];
	module->domain = moduleInf[1];
	module->label = moduleInf[2];
	std::cout << "HWFService::createModule: " << module->label << std::endl;
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
			std::cout << "createModuleTopology in for: " << *i << std::endl;
			std::vector<std::string> connectorRawData;
			boost::split(connectorRawData, *i, boost::is_any_of(":"));
			std::vector<std::string> connectorData;
			boost::split(connectorData, connectorRawData[1], boost::is_any_of(","));
			std::cout << "createModuleTopology: connectorData size: " << connectorData.size() << std::endl;
			CONNECTOR* conn = new CONNECTOR(std::stoi(connectorData[0]));
			conn->label = connectorData[1];
			conn->used = true;
			conn->type = static_cast<CONNECTOR::EType>(std::stoi(connectorData[2]));
			connectorsGroup.push_back(conn);
		}
		if ((*i).find("Antenna") != std::string::npos)
		{
			std::cout << "createModuleTopology in for: " << *i << std::endl;
			std::vector<std::string> connectorRawData;
			boost::split(connectorRawData, *i, boost::is_any_of(":"));
			std::vector<std::string> connectorData;
			boost::split(connectorData, connectorRawData[1], boost::is_any_of(","));
			std::cout << "createModuleTopology: connectorData size: " << connectorData.size() << std::endl;
			ANTENNA* antenna = new ANTENNA();
			antenna->id = std::stoi(connectorData[0]);
			antenna->label = connectorData[1];
			connectorsGroup.push_back(antenna);
		}
	}
	moduleObj->connectors_.push_back(connectorsGroup);
}

