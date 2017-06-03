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

#include "HWPlannerService.hpp"
#include "HWFService.hpp"
#include "Objects\Obj.hpp"
#include "EQM.hpp"
#include "MODULE.hpp"
#include "CONNECTOR.hpp"
#include "TIMER.hpp"
#include "TASK.hpp"
#include "EthernetIntrfaceConfigurator.hpp"

class Router
{
public:
	typedef Component* Component_ptr;
	Router(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger);
	~Router();
	void receiver(std::string data);
	void sender(std::string data);
	void startHWPlanerService();
	
private:
	HWPlannerService* hwplannerService_;
	boost::log::sources::logger_mt logger_;
	std::vector<Component_ptr> components_;
	std::vector<std::string> mmfS_;
	std::string mmfPath_;
	std::vector<Obj*>* cache_;
	bool fabricStartup_;
	EQM* eqmObj_;
	TIMER * timer_;
	bool timeout_;
	boost::thread hwPlannerServiceThread_;
	TASK* task;
	HWFService* hwfService_;
	EthernetIntrfaceConfigurator* ethIntConfigurator_;

	void startAutodetection();
	void startInternalModuleAutodetection();
	void startComponentService();
	void startComponent(std::string name, std::string address);
	void initializeComponent(std::string name);
	void checkIfMMFExists();
	void createEQM();
	void moduleAutodetection(std::string domain);
	void setupModule(Obj* mod);
	void createConnectors(MODULE* mod);
	void displayModulesTopology();
	void setupTimer();
	void checkResultFromHWPlannerService();
	void createMMFFromEQM();

};

