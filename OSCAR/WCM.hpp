#pragma once
#include "Component.hpp"
#include "REMOTE_CONTROLLER.hpp"
#include "BDM.hpp"
#include "CARD_PORT.hpp"
#include "ALARM.hpp"
#include "UserManager.hpp"
#include "EthernetIntrfaceConfigurator.hpp"
#include "MODULE.hpp"
#include "EQM.hpp"
#include "ANTENNA.h"
#include "rcManager.hpp"

class WCM :
	public Component
{
public:
	WCM(std::string domain, boost::log::sources::logger_mt logger);
	~WCM();
	void execute(std::string message);
	void execute(INTER_MODULE_OPERATION* imo);
	CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg) { return nullptr; };
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(std::string)> func) { send = func; }
	void setup(std::string domain) {};
	RESULT* setup(int domain) { return new RESULT(); }
	void initialize();
private:
	boost::log::sources::logger_mt logger_;
	REMOTE_CONTROLLER* rcObj_;
	CARD_PORT* cardPortObj_;
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	MODULE* wcmModule_;
	std::vector<ANTENNA*> antenaDevices_;
	UserManager* userManager_;
	EthernetIntrfaceConfigurator* ethIntConfigurator_;
	rcManager* rcMan_;
	BDM* bdmObjPtr_;
	EQM* eqmObjPtr_;
	void ethernetInitialize();
	void prepareTopology();
	void getAllAntenaDecives();
	void getWCM();
	void getBDM();
	std::string WCM::getSerialNumberFromRefRCObj();
	void executeOnUIA(INTER_MODULE_OPERATION* imo);
	void displayTopology();
	void getEQM();
	void startWebUIServiceIsNeeded();
};

