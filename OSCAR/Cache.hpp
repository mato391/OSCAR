#pragma once
#include <vector>
#include <string>
#include "Objects\Obj.hpp"
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include "MODULE.hpp"
class Cache
{
public:
	Cache(boost::log::sources::logger_mt logger);
	~Cache();
	int addObject(Obj* obj);
	int removeObj(Obj* obj);
	std::vector<Obj*> getAllObjects(std::string name);
	std::vector<Obj*> getAllObjectsFromChildren(std::string parentName, std::string name);
	std::vector<Obj*> getAllObjectsFromGrandChildren(std::string grandName, std::string parentName, std::string name);
	Obj* getUniqueObject(std::string name);
	Obj* getUniqueObjectFromChildren(std::string parentName, std::string childName);
	Obj* getUniqueObjectFromGrandChildren(std::string grandName, std::string parentName, std::string childName);
private:
	std::vector<Obj*> cache_;
	boost::log::sources::logger_mt logger_;
	
};

