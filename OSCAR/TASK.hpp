#pragma once
#include "D:\private\OSCAR\New_Architecture_OSCAR\OSCAR\OSCAR\Objects\Obj.hpp"
#include <functional>
class TASK :
	public Obj
{
public:
	TASK(std::string taskFor, std::function<void()> feedback);
	~TASK();
	std::string taskFor;
	std::function<void()> feedback;
};

