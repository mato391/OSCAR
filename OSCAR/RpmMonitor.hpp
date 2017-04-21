#pragma once
#include "ENGINE.hpp"
#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <boost\filesystem.hpp>
#include <boost\thread.hpp>
class RpmMonitor
{
public:
	RpmMonitor(ENGINE* engineObjPtr, boost::log::sources::logger_mt logger);
	~RpmMonitor();
	void start();
private:
	ENGINE* engineObjPtr_;
	boost::log::sources::logger_mt logger_;
	const std::string rpmFileStreamPath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\rpm.txt";
};

