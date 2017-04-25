#pragma once

#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <vector>
#include "Objects\Obj.hpp"

class MirrorModule
{
public:
	MirrorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger);
	~MirrorModule();
private:
	std::vector<Obj*>* cache_;
	boost::log::sources::logger_mt logger_;
	void createMirrors();
};

