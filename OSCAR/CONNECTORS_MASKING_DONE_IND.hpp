#pragma once
#include "Objects\Obj.hpp"
#include "CONNECTOR.hpp"

class CONNECTORS_MASKING_DONE_IND :
	public Obj
{
public:
	CONNECTORS_MASKING_DONE_IND();
	~CONNECTORS_MASKING_DONE_IND();
	std::vector<Obj*> connectors_;
	std::string domain;
};

