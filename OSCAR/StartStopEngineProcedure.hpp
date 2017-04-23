#pragma once

#include "ENGINE.hpp"
#include "RESULT.hpp"
#include <assert.h>
#include <fstream>
#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class StartStopEngineProcedure
{
public:
	StartStopEngineProcedure(ENGINE* engineObjPtr, boost::log::sources::logger_mt logger);
	~StartStopEngineProcedure();
	RESULT getResult() { return *result; }
private:
	ENGINE* engineObjPtr_;
	boost::log::sources::logger_mt logger_;
	void determineAction();
	void startProcedure();
	void stopProcedure();
	RESULT* result;

	const std::vector<std::string> proceduresAddresses_ = { "0x0302", "0x0303" };
};

