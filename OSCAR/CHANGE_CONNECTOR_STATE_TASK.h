#pragma once
#include "MODULE_TASK.hpp"
class CHANGE_CONNECTOR_STATE_TASK :
	public MODULE_TASK
{
public:
	CHANGE_CONNECTOR_STATE_TASK(int port, int value);
	~CHANGE_CONNECTOR_STATE_TASK();
	int port;
	int value;
};

