#pragma once

#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

#include "Component.hpp"
#include "BDM.hpp"
#include "WCM.hpp"
#include "UCM.hpp"
#include "EDM.hpp"
#include "UIA.hpp"

class ComponentFactory
{
public:
	static Component* createComponent(std::string name, std::string domain, boost::log::sources::logger_mt logger);
};

