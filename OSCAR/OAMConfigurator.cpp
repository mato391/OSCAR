#include "stdafx.h"
#include "OAMConfigurator.hpp"


OAMConfigurator::OAMConfigurator(boost::log::sources::logger_mt logger)
{
	logger_ = logger;
	BOOST_LOG(logger_) << "OAMConfigurator ctor";
	prepareCP();
	cachePtr_ = new Cache(logger);
}


OAMConfigurator::~OAMConfigurator()
{
}

void OAMConfigurator::prepareCP()
{
	std::fstream cpFile("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\CP.txt", std::ios::in);
	std::string cpFileContent;
	cpFile >> cpFileContent;
	cpFile.close();
	std::vector<std::string> cpFileContentList;
	boost::split(cpFileContentList, cpFileContent, boost::is_any_of(";"));
	createCPFile(cpFileContentList);
}

void OAMConfigurator::createCPFile(std::vector<std::string> cpFileContentList)
{
	cpObj_ = new CP();
	for (const auto &param : cpFileContentList)
	{
		std::vector<std::string> paramSplitted;
		boost::split(paramSplitted, param, boost::is_any_of(":"));
		if (paramSplitted[0] == "DOOR_VERSION")
		{
			cpObj_->doorsVersion = std::stoi(paramSplitted[1]);
		}
		if (paramSplitted[0] == "LIGHT_VERSION")
		{
			cpObj_->lightVersion = std::stoi(paramSplitted[1]);
		}
		if (paramSplitted[0] == "AUTO_CLOSE_WIN")
		{
			cpObj_->autoClosingWindow = static_cast<bool>(std::stoi(paramSplitted[1]));
		}
	}
	cache_.push_back(cpObj_);
}
