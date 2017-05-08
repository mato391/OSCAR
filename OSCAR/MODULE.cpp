#include "stdafx.h"
#include "MODULE.hpp"


MODULE::MODULE()
{
	name = "MODULE";
}


MODULE::~MODULE()
{
}

void MODULE::addConnector(int connectorsInGroup)
{
	std::vector<Obj*> conns;
	for (int i = 0; i < connectorsInGroup; i++)
	{
		conns.push_back(new CONNECTOR(i));
	}
	connectors_.push_back(conns);

}