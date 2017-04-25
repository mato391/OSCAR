#include "stdafx.h"
#include "MirrorModule.hpp"


MirrorModule::MirrorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger)
{
	cache_ = cache;
	logger_ = logger;
	createMirrors();
}


MirrorModule::~MirrorModule()
{
}

void MirrorModule::createMirrors()
{

}