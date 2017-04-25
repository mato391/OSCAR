#include "stdafx.h"
#include "MIRROR.hpp"


MIRROR::MIRROR()
{
	name = "MIRROR";
}


MIRROR::~MIRROR()
{
}

void MIRROR::open()
{
	openingState = EOpeningState::opened;
}

void MIRROR::close()
{
	openingState = EOpeningState::closed;
}
