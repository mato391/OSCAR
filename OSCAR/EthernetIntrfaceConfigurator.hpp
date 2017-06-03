#pragma once
#include <vector>
#include "Objects\Obj.hpp"
#include "NET_INTERFACE.hpp"
#include "MODULE.hpp"
#include "EQM.hpp"

#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <boost\filesystem.hpp>


#include <fstream>

class EthernetIntrfaceConfigurator
{
public:
	EthernetIntrfaceConfigurator(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger);
	~EthernetIntrfaceConfigurator();
	void getEthernetInformation();
private:
	MODULE* wcmModule_;
	std::vector<Obj*>* cache_;
	boost::log::sources::logger_mt logger_;

	void getWCMModule();
	void createNetworkInformations(std::string data);
};

