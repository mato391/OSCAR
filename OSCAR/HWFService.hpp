#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <boost\algorithm\string.hpp>
#include "MODULE.hpp"
#include "CONNECTOR.hpp"
#include "EQM.hpp"

class HWFService
{
public:
	HWFService(EQM* eqmObjPtr);
	~HWFService();
	void prepareTopology();
private:
	const std::string hwfPath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\hwf.txt";
	void loadHwf();
	EQM* eqmObjPtr_;
	MODULE* createModule(std::string data);
	void createModuleTopology(MODULE* moduleObj, std::string data);
	std::string hwfFileContent_;
};
