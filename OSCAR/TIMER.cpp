#include "stdafx.h"
#include "TIMER.hpp"
#include <iostream>

TIMER::TIMER(int timeout, std::string label)
{
	this->timeout = timeout;
	this->label = label;
}


TIMER::~TIMER()
{
}

void TIMER::start(bool * feedback)
{
	std::cout << "TIMER STARTED" << std::endl;
	boost::this_thread::sleep_for(boost::chrono::seconds(timeout));
	*feedback = true;
	std::cout << "TIMER STOPPED" << std::endl;
}
