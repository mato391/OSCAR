#pragma once

#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <vector>
#include <bitset>
#include "Objects\Obj.hpp"
#include "MODULE.hpp"
#include "MIRROR.hpp"
#include "MIRRORS.hpp"
#include "DOORS.hpp"
#include "CONNECTORS_MASKING_DONE_IND.hpp"
#include "MASK_CONNECTORS_STATE.hpp"
#include "Cache.hpp"
#include <boost\thread.hpp>
#include "swconfig.hpp"
#include "ActionSet.hpp"

class MirrorModule
{
public:
	MirrorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr);
	~MirrorModule();
	void setMirrorPosition(int x, int y, int z, int label);
	
	void initialize();
	void setup();
	int getModuleProtocol() { return static_cast<int>(mirrorModule_->protocol); };
private:
	std::vector<Obj*>* cache_;
	Cache* cachePtr_;
	boost::log::sources::logger_mt logger_;
	MIRRORS* mirrorsObj_;
	MODULE* mirrorModule_;
	DOORS doors_;
	int doorsSubscId_;
	int cmdiSubscrId_;
	ActionSet* actionSet_;

	void getBDMModule();
	void createMirrors();
	void displayTopology();
	bool checkIfMotorOutExist();
	void handleDoorsStateChange(Obj* obj);
	void handleModuleTask(Obj* obj);
	std::pair<int, int> createMaskForConnectorChange(std::vector<int> portsId, std::vector<int> portsValues);
	int getConnIdByLabel(std::string label);
	std::vector<int> getPortsIdForMirrorClosing();
	void getConnectorsIdsForDefualtActions();

	void onDoorsUnlock();
	void onDoorsLock();
	void handleConnectorMaskingInd(Obj * obj);
	
};

