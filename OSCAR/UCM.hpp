#pragma once
#include "Component.hpp"
#include <fstream>
#include "SCF.hpp"
#include "SWITCH_TOPOLOGY.hpp"
#include "BUTTON.hpp"
#include "PEDAL.h"
#include "ENGINE.hpp"
#include "RESULT.hpp"
#include "StartStopEngineButtonAgent.hpp"
class UCM :
	public Component
{
public:
	UCM(std::string domain, boost::log::sources::logger_mt logger);
	~UCM();
	void execute(std::string message);
	void execute(INTER_MODULE_OPERATION* imo);
	CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg) { return nullptr; };
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(CMESSAGE::CMessage*)> func) { send = func; }
	void setup(std::string domain) {}
	RESULT* setup(int domain) { return new RESULT(); }
private:
	SCF* scfObj_;
	SWITCH_TOPOLOGY* switchTopology;
	ENGINE* engineObjPtr_;
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	std::vector<std::string> scfVec_;
	StartStopEngineButtonAgent* startStopEngineButtonAgent_;

	void initialize();
	void prepareSwitchTopology();
	void prepareTopologyBasedOnSCF();
	void startUserOperationProcessing(BUTTON* button);
	int checkPreconditionToStartEngine();
	void displayTopology();
	PEDAL* getPedalFromTopology(std::string label);
	void startPedalOperation(PEDAL* pedal);
	RESULT* getOperationResult();
	Component* getComponent(std::string label);

};

