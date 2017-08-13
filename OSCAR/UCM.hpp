#pragma once
#include "Component.hpp"
#include "Cache.hpp"
#include <fstream>
#include "SCF.hpp"
#include "MODULE.hpp"
#include "SWITCH_TOPOLOGY.hpp"
#include "BUTTON.hpp"
#include "PEDAL.h"
#include "ENGINE.hpp"
#include "RESULT.hpp"
#include "StartStopEngineButtonAgent.hpp"
#include "CHANGE_BUTTON_STATE_IND.hpp"
#include "CHANGE_CONNECTOR_DONE_IND.hpp"

#include <boost\algorithm\string.hpp>
class UCM :
	public Component
{
public:
	UCM(std::string domain, boost::log::sources::logger_mt logger);
	~UCM();
	void execute(std::string message);
	void execute(INTER_MODULE_OPERATION* imo);
	CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg);
//	CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg) { return nullptr; };
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	//void setCache(Cache* cache) { cachePtr_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(CMESSAGE::CMessage*)> func) { send = func; }
	void setup(std::string domain) {}
	RESULT* setup(int domain);
private:
	SCF* scfObj_;
	SWITCH_TOPOLOGY* switchTopology;
	ENGINE* engineObjPtr_;
	MODULE* ucmModuleObj_;
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	std::vector<std::string> scfVec_;
	StartStopEngineButtonAgent* startStopEngineButtonAgent_;
	//Cache* cachePtr_;

	int ccdiSubscrId_;

	void initialize();
	void getUCMModule();
	void prepareSwitchTopology();
	void prepareTopologyBasedOnSCF();
	void startUserOperationProcessing(BUTTON* button);
	int checkPreconditionToStartEngine();
	void displayTopology();
	PEDAL* getPedalFromTopology(std::string label);
	void startPedalOperation(PEDAL* pedal);
	RESULT* getOperationResult();
	Component* getComponent(std::string label);

	void handleConnectorChange(Obj* obj);
};

