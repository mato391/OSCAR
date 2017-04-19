#pragma once
#include "Component.hpp"
#include "REMOTE_CONTROLLER.hpp"
#include "BDM.hpp"
#include "CARD_PORT.hpp"
#include "ALARM.hpp"

class WCM :
	public Component
{
public:
	WCM(std::string domain, boost::log::sources::logger_mt logger);
	~WCM();
	void execute(std::string message);
	void execute(INTER_MODULE_OPERATION* imo);
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(std::string)> func) { send = func; }
	void initialize();
private:
	REMOTE_CONTROLLER* rcObj_;
	CARD_PORT* cardPortObj_;
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	BDM* bdmObjPtr_;

	void getBDM();
	std::string WCM::getSerialNumberFromRefRCObj();
};

