#include "stdafx.h"
#include "RpmMonitor.hpp"


RpmMonitor::RpmMonitor(ENGINE* engineObjPtr, boost::log::sources::logger_mt logger)
{
	engineObjPtr_ = engineObjPtr;
	logger_ = logger;
	interupt = false;
	BOOST_LOG(logger_) << "INFO " << "RpmMonitor ctor";
}


RpmMonitor::~RpmMonitor()
{
	BOOST_LOG(logger_) << "INFO " << "RpmMonitor dtor";
	boost::filesystem::remove(rpmFileStreamPath_);
	std::fstream rpmDump("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Logs\\rpmDump.txt", std::ios::out);
	for (const auto &rpm : rpmDump_)
	{
		rpmDump << rpm;
		rpmDump << "\n";
	}
	rpmDump.close();
	
}

void RpmMonitor::start()
{
	BOOST_LOG(logger_) << "INFO " << boost::this_thread::get_id << " RpmMonitor::start";
	while (!interupt)
	{
		if (boost::filesystem::exists(rpmFileStreamPath_))
		{
			std::fstream rpmStream(rpmFileStreamPath_, std::ios::in);
			std::string rpm;
			rpmStream >> rpm;
			rpmStream.close();
			boost::filesystem::remove(rpmFileStreamPath_);
			BOOST_LOG(logger_) << "DBG " << boost::this_thread::get_id << " RPM change: " << rpm;
			engineObjPtr_->rpm = std::stoi(rpm);
			rpmDump_.push_back(rpm);
		}
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
}