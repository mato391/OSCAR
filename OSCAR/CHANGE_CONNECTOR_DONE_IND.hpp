#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
#include "CONNECTOR.hpp"
class CHANGE_CONNECTOR_DONE_IND :
	public Obj
{
public:
	CHANGE_CONNECTOR_DONE_IND();
	~CHANGE_CONNECTOR_DONE_IND();
	std::string domain;
	CONNECTOR* connector;
};

