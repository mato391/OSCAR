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
#include <boost\thread\thread.hpp>
#include "HWPlannerService.hpp"
#include "HWFService.hpp"
#include "Objects\Obj.hpp"
#include "EQM.hpp"
#include "MODULE.hpp"
#include "CONNECTOR.hpp"
#include "TIMER.hpp"
#include "TASK.hpp"
#include "Cache.hpp"
#include "EthernetIntrfaceConfigurator.hpp"
#include "ModuleInitialConfigurator.hpp"
#include "CAN.h"
#include "ProtocolManager.h"
#include "swconfig.hpp"


#define BB 187
#define BC 188

class Router
{
public:
	typedef Component* Component_ptr;
	Router(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr);
	~Router();
	void receiver(std::string data);
	void sender(CMESSAGE::CMessage* msg);
	//void startHWPlanerService();
	//std::vector<Obj*> getModules();
	//void setHwSimulatorIfNeeded(HwSimulator* hwSim) { this->hwSim_ = hwSim; };
private:
	//HWPlannerService* hwplannerService_;
	boost::log::sources::logger_mt logger_;
	std::vector<Component_ptr> components_;
	std::vector<std::string> mmfS_;
	std::string mmfPath_;
	std::vector<Obj*>* cache_;
	Cache* cachePtr_;
	bool fabricStartup_;
	EQM* eqmObj_;
	bool timeout_;
	//boost::thread hwPlannerServiceThread_;
	
	HWFService* hwfService_;
	ModuleInitialConfigurator* mIC_;
	CAN * canPtr_;
	ProtocolManager* protoManager_;
	std::string modLabel_;
	void startAutodetection();
	void startComponentService();
	void startComponent(std::string name, std::string address);
	void checkIfMMFExists();
	void createEQM();
	void setupModule(std::string domain, int mask);
	boost::thread_group componentsThreadGroup_;
};

