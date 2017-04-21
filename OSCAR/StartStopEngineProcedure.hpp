#pragma once

#include "ENGINE.hpp"
#include "RESULT.hpp"
#include <assert.h>
#include <fstream>
class StartStopEngineProcedure
{
public:
	StartStopEngineProcedure(ENGINE* engineObjPtr);
	~StartStopEngineProcedure();
	RESULT getResult() { return *result; }
private:
	ENGINE* engineObjPtr_;
	void determineAction();
	void startProcedure();
	void stopProcedure();
	RESULT* result;
	const std::vector<std::string> proceduresAddresses_ = { "0x0302", "0x0303" };
};

