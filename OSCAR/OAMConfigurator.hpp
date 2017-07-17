#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <boost\algorithm\string.hpp>
#include <fstream>
#include "Objects\CP.hpp"
#include "Objects\Obj.hpp"
#include "Cache.hpp"
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class OAMConfigurator
{
public:
	OAMConfigurator(boost::log::sources::logger_mt logger);
	~OAMConfigurator();
	CP* getConfiguration() { return cpObj_; }
	std::vector<Obj*>* getObjectCachePtr() { return &cache_; }
	Cache* getCachePtr() { return cachePtr_; }
private:
	boost::log::sources::logger_mt logger_;
	CP* cpObj_;
	std::vector<Obj*> cache_;
	void prepareCP();
	void createCPFile(std::vector<std::string> cpFileContentList);
	Cache* cachePtr_;
};

