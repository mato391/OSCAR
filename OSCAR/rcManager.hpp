#pragma once
#include <vector>
#include "Objects\Obj.hpp"
#include "NET_INTERFACE.hpp"
#include "EQM.hpp"
#include "MODULE.hpp"
#include "ANTENNA.h"
#include "REMOTE_CONTROLLER.hpp"
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class rcManager
{
public:
	rcManager(MODULE* wcmModule, boost::log::sources::logger_mt logger);
	~rcManager();
	void prepareRCInterface();
private:
	boost::log::sources::logger_mt logger_;
	MODULE* wcmModule_;
	REMOTE_CONTROLLER* rControler_;
	void configureAdvancedRemoteController();

};

