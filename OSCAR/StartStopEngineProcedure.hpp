#pragma once

#include "ENGINE.hpp"

class StartStopEngineProcedure
{
public:
	StartStopEngineProcedure(ENGINE* engineObjPtr);
	~StartStopEngineProcedure();
private:
	ENGINE* engineObjPtr_;
};

