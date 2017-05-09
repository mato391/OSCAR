#pragma once
#include "Objects\Obj.hpp"
#include <boost\thread.hpp>
class TIMER :
	public Obj
{
public:
	TIMER(int timeout, std::string label);
	~TIMER();
	std::string label;
	int timeout;

	void start(bool *feedback);
};

