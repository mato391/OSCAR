#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
class NET_INTERFACE :
	public Obj
{
public:
	NET_INTERFACE();
	~NET_INTERFACE();
	std::string ifName;
	std::string type;
	std::string ip;
	std::string mask;
	bool dhcp;
	std::string gateway;
};

