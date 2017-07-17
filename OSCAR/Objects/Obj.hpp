#pragma once
#include <string>
#include <vector>
class Obj
{
public:
	std::string name;
	std::vector<Obj*> children;
};

