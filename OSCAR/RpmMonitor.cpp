#include "stdafx.h"
#include "RpmMonitor.hpp"


RpmMonitor::RpmMonitor(ENGINE* engineObjPtr, boost::log::sources::logger_mt logger)
{
	engineObjPtr_ = engineObjPtr;
	logger_ = logger;
	BOOST_LOG(logger_) << "INFO " << "RpmMonitor ctor";
}


RpmMonitor::~RpmMonitor()
{
}

void RpmMonitor::start()
{
	BOOST_LOG(logger_) << "INFO " << boost::this_thread::get_id << " RpmMonitor::start";
	while (true)
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
		}
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
}