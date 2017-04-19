#pragma once
#include "Component.hpp"
#include <vector>
#include <string>
#include <fstream>

#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <boost\filesystem.hpp>

#include "Objects\Obj.hpp"

class Router
{
public:
	typedef Component* Component_ptr;
	Router(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger);
	~Router();
	void receiver(std::string data);
	void sender(std::string data);
	
private:
	boost::log::sources::logger_mt logger_;
	std::vector<Component_ptr> components_;
	std::vector<std::string> mmfS_;
	std::string mmfPath_;
	std::vector<Obj*>* cache_;
	void startComponentService();
	void startComponent(std::string name, std::string address);
};
