#pragma once
#include "Objects\Obj.hpp"
#include <vector>
#include <map>
class MMF :
	public Obj
{
public:
	MMF();
	~MMF();
	std::vector<std::map<std::string, std::string>> modules_;
};

