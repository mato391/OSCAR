#include "stdafx.h"
#include "ComponentFactory.hpp"


Component* ComponentFactory::createComponent(std::string name, std::string domain, boost::log::sources::logger_mt logger)
{
	std::cout << "ComponentFactory " << name << " " << domain << std::endl;
	if (name.find("BDM") != std::string::npos)
	{
		std::cout << "ComponentFactory returning new BDM" << std::endl;
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
	else if (name == "IDM")
		return new IDM(domain, logger);

}
