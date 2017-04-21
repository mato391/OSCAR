#include "stdafx.h"
#include "StartStopEngineProcedure.hpp"


StartStopEngineProcedure::StartStopEngineProcedure(ENGINE* engineObjPtr)
{
	engineObjPtr_ = engineObjPtr;
	determineAction();
}


StartStopEngineProcedure::~StartStopEngineProcedure()
{
}

void StartStopEngineProcedure::determineAction()
{
	if (engineObjPtr_ != nullptr &&
		engineObjPtr_->proceduralState == ENGINE::EProceduralState::configured)
	{
		startProcedure();
	}
	else if (engineObjPtr_ != nullptr &&
		(engineObjPtr_->proceduralState == ENGINE::EProceduralState::startedIdle) ||
		engineObjPtr_->proceduralState == ENGINE::EProceduralState::startedLoaded)
	{
		stopProcedure();
	}
	assert(engineObjPtr_ != nullptr);
	
}
void StartStopEngineProcedure::startProcedure()
{
	result = new RESULT();
	result->applicant = "StartStopEngineProcedure";
	result->status = RESULT::EStatus::success;
	result->feedback = proceduresAddresses_[0];
}

void StartStopEngineProcedure::stopProcedure()
{
	result = new RESULT();
	result->applicant = "StartStopEngineProcedure";
	result->status = RESULT::EStatus::success;
	result->feedback = proceduresAddresses_[1];
}
