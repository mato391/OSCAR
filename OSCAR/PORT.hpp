#pragma once
#include "Objects\Obj.hpp"
class PORT :
	public Obj
{
public:
	PORT();
	~PORT();
	bool isUsed;
	enum class EType
	{};
};

