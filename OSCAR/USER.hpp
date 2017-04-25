#pragma once
#include "Objects\Obj.hpp"
#include <vector>

class USER :
	public Obj
{
public:
	USER();
	~USER();
	std::string username;
	std::string rcSerialCode;
	std::vector<Obj*> children;
	bool isNew;
	
};

