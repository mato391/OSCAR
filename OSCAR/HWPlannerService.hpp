#pragma once
#include <string>
#include <iostream>
#include <fstream>

#include "Objects\Obj.hpp"
#include "EQM.hpp"
#include "MODULE.hpp"
#include "CONNECTOR.hpp"

#include <boost\thread.hpp>
#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <boost/filesystem.hpp>

class HWPlannerService
{
public:
	HWPlannerService(boost::log::sources::logger_mt logger, std::vector<Obj*>* cache);
	~HWPlannerService();
	void startReceiving();
private:
	const std::string commStreamRecvPath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\hwps_recv.txt";
	const std::string hwfFilePath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\hwf.txt";
	boost::log::sources::logger_mt logger_;
	std::string hwfContent_;
	std::vector<Obj*>* cache_;
	EQM* eqmObjPtr_;

	void handleMessage(std::string message);
	void loadHWF();
	void createMMF();
	void createModule(std::string mod);
	void setConnector(int id, std::string label, int connectorGroup, std::string moduleSn);
};

