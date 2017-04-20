#include "stdafx.h"
#include "StartStopEngineProcedure.hpp"


StartStopEngineProcedure::StartStopEngineProcedure(ENGINE* engineObjPtr) :
	engineObjPtr_(engineObjPtr)
{
	determineAction()
}


StartStopEngineProcedure::~StartStopEngineProcedure()
{
}
