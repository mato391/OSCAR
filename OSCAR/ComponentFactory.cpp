#include "stdafx.h"
#include "ComponentFactory.hpp"


Component* ComponentFactory::createComponent(std::string name, std::string domain, boost::log::sources::logger_mt logger)
{
	if (name == "BDM")
	{
		return new BDM(domain, logger);
	}
	else if (name == "WCM")
	{
		return new WCM(domain, logger);
	}
	else if (name == "UCM")
	{
		return new UCM(domain, logger);
	}
	else if (name == "EDM")
	{
		return new EDM(domain, logger);
	}
	else if (name == "UIA")
		return new UIA(domain, logger);

}
