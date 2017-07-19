#include "stdafx.h"
#include "CHANGE_CONNECTOR_STATE_TASK.h"


CHANGE_CONNECTOR_STATE_TASK::CHANGE_CONNECTOR_STATE_TASK(int port, int value) 
{
	this->type = EName::CHANGE_CONNECTOR_STATE_TASK;
	this->port = port;
	this->value = value;
}


CHANGE_CONNECTOR_STATE_TASK::~CHANGE_CONNECTOR_STATE_TASK()
{
}
