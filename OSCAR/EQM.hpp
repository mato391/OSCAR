#pragma once
#include "Objects\Obj.hpp"
#include <vector>
class EQM :
	public Obj
{
public:
	EQM();
	~EQM();
	void addModule(Obj* obj);
	std::vector<Obj*> modules_;
	enum class EConfiguringState
	{
		naked,
		configured
	};
	EConfiguringState configuringState;
};

