#pragma once
#include "Objects\Obj.hpp"
#include "CONNECTOR.hpp"
#include <vector>
class MODULE :
	public Obj
{
public:
	MODULE();
	~MODULE();
	void addConnector(int connectorsInGroup);
	std::vector<std::vector<Obj*>> connectors_;
	std::vector<int> connectorGroups_;
	std::string domain;
	std::string mask;
	std::string serialNumber;
	std::string productNumber;
	std::string label;
};

