#pragma once
#include "Component.hpp"
#include <fstream>
#include "SCF.hpp"
#include "SWITCH_TOPOLOGY.hpp"
#include "BUTTON.hpp"
#include "PEDAL.h"

class UCM :
	public Component
{
public:
	UCM(std::string domain, boost::log::sources::logger_mt logger);
	~UCM();
	void execute(std::string message);
	void execute(INTER_MODULE_OPERATION* imo);
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(std::string)> func) { send = func; }
private:
	SCF* scfObj_;
	SWITCH_TOPOLOGY* switchTopology;
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	std::vector<std::string> scfVec_;

	void initialize();
	void prepareSwitchTopology();
	void prepareTopologyBasedOnSCF();
	void startUserOperationProcessing(BUTTON* button);
	int checkPreconditionToStartEngine();
	void displayTopology();
	PEDAL* getPedalFromTopology(std::string label);

};

