#pragma once
#include "Objects\Obj.hpp"
#include "RESULT.hpp"
class MODULE_TASK :
	public Obj
{
public:
	MODULE_TASK();
	~MODULE_TASK();
	enum class EName
	{
		CHANGE_CONNECTOR_STATE_TASK,
		LIGHT_WELCOMING_TASK,
		LIGHT_GOODBYE_TASK
	};
	EName type;
	RESULT* result;
	RESULT* getResult() { return result; }
	std::string taskFor;

};

