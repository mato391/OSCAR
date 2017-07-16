// OSCAR.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <signal.h>
#include "Router.hpp"
#include "OAMConfigurator.hpp"
#include <boost/log/trivial.hpp>
#include <boost/move/utility.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/filesystem.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::logger_mt)

void router_thread(Router* router, src::logger_mt lg);


int main()
{
	src::logger_mt& lg = my_logger::get();
	logging::add_file_log
	(
		keywords::file_name = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Logs\\SYSLOG_%N.log",
		keywords::rotation_size = 10 * 1024 * 1024,
		keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = "[%TimeStamp%]: %Message%",
		keywords::auto_flush = true
	);
	logging::add_common_attributes();
	logging::record rec = lg.open_record();

	BOOST_LOG(lg) << "INFO " << "OSCAR: " << "Application started";

	OAMConfigurator* oamConfigurator = new OAMConfigurator(lg);
	auto cachePtr = oamConfigurator->getObjectCachePtr();
	if (cachePtr == nullptr)
		BOOST_LOG(lg) << "ERR " << "Cache is empty";
	Router* router = new Router(cachePtr, lg);
	BOOST_LOG(lg) << "INF " << "OSCAR: -------------------SYSTEM STARTED---------------------";
	router_thread(router, lg);
	
    return 0;
}

void router_thread(Router* router, src::logger_mt lg)
{
	BOOST_LOG(lg) << "INF " << "OSCAR: router listening";
	for (;;)
	{
		router->receiver("");
		boost::this_thread::sleep(boost::posix_time::millisec(100));
	}
	BOOST_LOG(lg) << "ERR " << "OSCAR: router stoped listening";
}



