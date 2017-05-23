#pragma once
#include "Objects\Obj.hpp"
#include "CONNECTOR.hpp"
#include <vector>
class PORT :
	public Obj
{
public:
	PORT();
	~PORT();
	std::vector<CONNECTOR*> connectors;
	bool isUsed;
	enum class EType
	{};
	std::string label;
};

