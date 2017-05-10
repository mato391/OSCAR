#include "stdafx.h"
#include "TASK.hpp"


TASK::TASK(std::string taskFor, std::function<void()> feedback)
{
	name = "TASK";
	this->taskFor = taskFor;
	this->feedback = feedback;
}


TASK::~TASK()
{
}
