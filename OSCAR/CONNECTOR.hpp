#pragma once
#include "Objects\Obj.hpp"
class CONNECTOR :
	public Obj
{
public:
	CONNECTOR(int id);
	~CONNECTOR();
	int id;
	bool used;
};

