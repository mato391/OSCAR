#pragma once
#include "Component.hpp"
#include "ENGINE.hpp"
#include <fstream>
#include <boost\thread.hpp>
#include <boost\asio.hpp>
#include "EngineCommunicationService.hpp"
#include "StartStopEngineProcedure.hpp"
#include "RESULT.hpp"
#include "RpmMonitor.hpp"
using boost::asio::ip::tcp;

class EDM :
	public Component
{
public:
	EDM(std::string domain, boost::log::sources::logger_mt logger);
	~EDM();
	void execute(std::string message);
	CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg) { return nullptr; };
	void execute(INTER_MODULE_OPERATION* imo);
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(CMESSAGE::CMessage*)> func) { send = func; }
	void setup(std::string domain) {};
	int runtime() {};
	RESULT* setup(int domain) { return new RESULT(); };
private:
	ENGINE* engineObj_;
	void initialize();
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	StartStopEngineProcedure* startStopEngineProcedure_;
	RpmMonitor* rpmMonitor_;
	boost::thread rpmMonitorStart;

	bool checkPreconditionsToStartEngine();
	void checkIfEngineStarted(std::string data);
	void loadEngineMaps(int pos);
	void rpmMonitor();
	int getMilageFromPersist();
	void EDM::startDetection();
	std::vector<std::map<int, std::string>> alarmsDescriptions_;
	void setupEngine(ENGINE::EType type, ENGINE::EPetrolType petrolType);
	
};

