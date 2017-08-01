#pragma once
#include "MODULE_TASK.hpp"
class MASK_CONNECTORS_STATE :
	public MODULE_TASK
{
public:
	MASK_CONNECTORS_STATE();
	~MASK_CONNECTORS_STATE();
	int mask1;
	int mask2;
};

