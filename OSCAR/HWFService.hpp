#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

#include "MODULE.hpp"
#include "CONNECTOR.hpp"

#include "EQM.hpp"
#include "ANTENNA.h"
#include "SWITCH_CONNECTOR.hpp"
#include "BUTTON.hpp"
#include "Objects\DOOR.hpp"
#include "Cache.hpp"

class HWFService
{
public:
	HWFService(EQM* eqmObjPtr, boost::log::sources::logger_mt logger, Cache* cachePtr);
	~HWFService();
	void prepareTopology();
private:
	const std::string hwfPath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\hwf.txt";
	void loadHwf();
	Cache* cachePtr_;
	EQM* eqmObjPtr_;
	boost::log::sources::logger_mt logger_;
	MODULE* createModule(std::string data);
	void createModuleTopology(MODULE* moduleObj, std::string data);
	std::string hwfFileContent_;
	
};

