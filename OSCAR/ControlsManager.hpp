#pragma once
#include "Cache.hpp"
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <vector>
#include <functional>
#include <fstream>
#include "DOORS.hpp"
#include "Objects\LIGHT.hpp"

class ControlsManager
{
public:
	ControlsManager(Cache* cachePtr, boost::log::sources::logger_mt logger);
	~ControlsManager();
	void initialize();
private:
	Cache* cachePtr_;
	boost::log::sources::logger_mt logger_;
	DOORS doors_;
	std::vector<Obj> lightes_;

	
	void handleDoorsChange(Obj* obj);
	void handleLightChange(Obj* obj);

	std::fstream file_;
	std::vector<int> doorsSubsIds_;
	std::vector<int> lightsSubsIds_;
};

