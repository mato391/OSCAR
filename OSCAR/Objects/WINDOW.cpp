#include "stdafx.h"
#include "WINDOW.hpp"


WINDOW::WINDOW()
{
	name = "WINDOW";
}


WINDOW::~WINDOW()
{
}

void WINDOW::open()
{
	opened = true;
	//send to hwapi request for open window
}

void WINDOW::close()
{
	opened = false;
}
