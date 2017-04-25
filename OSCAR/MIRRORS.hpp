#pragma once
#include "Objects\Obj.hpp"
#include "MIRROR.hpp"
#include <vector>
class MIRRORS :
	public Obj
{
public:
	MIRRORS();
	~MIRRORS();
	std::vector<MIRROR*> children_;
};

