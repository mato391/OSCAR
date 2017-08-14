#include "stdafx.h"
#include "MirrorModule.hpp"
#define DEBUG true

MirrorModule::MirrorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr)
{
	cache_ = cache;
	cachePtr_ = cachePtr;
	logger_ = logger;
	actionSet_ = new ActionSet(logger);
	//createMirrors();
}

MirrorModule::~MirrorModule()
{
}

void MirrorModule::initialize()
{
	getBDMModule();
	createMirrors();
	getConnectorsIdsForDefualtActions();
}

void MirrorModule::getConnectorsIdsForDefualtActions()
{
	actionSet_->addAction(getPortsIdForMirrorClosing(), { 1, 0, 0, 1 }, "OPEN_MIRROR");
	actionSet_->addAction(getPortsIdForMirrorClosing(), { 0, 1, 1, 0 }, "CLOSE_MIRROR");
	//it will be more if it will be needed
}

void MirrorModule::setup()
{
	BOOST_LOG(logger_) << "INF " << "MirrorModule::setup " << std::endl;
	mirrorModule_->protocol = MODULE::EProtocol::CMaskProtocol;
	mirrorModule_->operationalState = MODULE::EOperationalState::enabled;
	doors_ = DOORS();
	if (checkIfMotorOutExist())
		doorsSubscId_ = cachePtr_->subscribe("DOORS", std::bind(&MirrorModule::handleDoorsStateChange, this, std::placeholders::_1), { 1 })[0];
	cmdiSubscrId_ = cachePtr_->subscribe("CONNECTORS_MASKING_DONE_IND", std::bind(&MirrorModule::handleConnectorMaskingInd, this, std::placeholders::_1), { 0 })[0];
}

void MirrorModule::handleConnectorMaskingInd(Obj * obj)
{
	auto cmdi = static_cast<CONNECTORS_MASKING_DONE_IND*>(obj);
	if (cmdi->domain == mirrorModule_->domain)
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " " << cmdi->connectors_.size();
		for (const auto &conn : cmdi->connectors_)
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			handleConnectorChange(conn);
		}
	}
}

void MirrorModule::handleConnectorChange(Obj* obj)
{
	if (obj->name != "CONNECTOR")
		return;
	auto conn = static_cast<CONNECTOR*>(obj);
	if (conn->label.find("COMMON"))
	{
		for (auto &mirror : mirrorsObj_->children)
		{
			auto mirrorC = static_cast<MIRROR*>(mirror);
			if (conn->value == 0
				&& mirrorC->openingState == MIRROR::EOpeningState::closed)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " changing state to opening";
				mirrorC->openingState = MIRROR::EOpeningState::opening;
			}
			else if (conn->value == 1
				&& mirrorC->openingState == MIRROR::EOpeningState::opening)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " changing state to opened";
				mirrorC->openingState = MIRROR::EOpeningState::opened;
			}
			else if (conn->value == 0
				&& mirrorC->openingState == MIRROR::EOpeningState::opened)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " changing state to closing";
				mirrorC->openingState = MIRROR::EOpeningState::closing;
			}
			else if (conn->value == 1
				&& mirrorC->openingState == MIRROR::EOpeningState::closing)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " changing state to closed";
				mirrorC->openingState = MIRROR::EOpeningState::closed;
			}
				
		}
	}
}

void MirrorModule::handleDoorsStateChange(Obj* obj)
{
	BOOST_LOG(logger_) << "INF " << "MirrorModule::handleDoorsStateChange";
	auto newDoors = static_cast<DOORS*>(obj);
	if (doors_.children.empty())
	{
		doors_ = *newDoors;
		if (doors_.lockingState == DOORS::ELockingState::locked)
			onDoorsLock();
		else if (doors_.lockingState == DOORS::ELockingState::unlocked)
			onDoorsUnlock();
	}
	else
	{
		if (doors_.lockingState != newDoors->lockingState)
		{
			doors_.lockingState = newDoors->lockingState;
			if (doors_.lockingState == DOORS::ELockingState::unlocked
				&& mirrorsObj_->openingState != MIRRORS::EOpeningState::opened)
				onDoorsUnlock();
			if (doors_.lockingState == DOORS::ELockingState::locked
				&& mirrorsObj_->openingState != MIRRORS::EOpeningState::opened)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MIRROS changing state to: readyToClose";
				mirrorsObj_->openingState = MIRRORS::EOpeningState::readyToClose;
			}
		}
		else if (doors_.lockingState == newDoors->lockingState &&
			mirrorsObj_->openingState == MIRRORS::EOpeningState::readyToClose)
			onDoorsLock();
	}
}

void MirrorModule::onDoorsUnlock()
{
	auto action = actionSet_->getAction("OPEN_MIRROR");
	auto mask = createMaskForConnectorChange(action->connIds, action->connValues);
	BOOST_LOG(logger_) << "INF " << "MirrorModule::handleDoorsStateChange: mirrors opening";
	auto res = new RESULT();
	res->applicant = "MIRROR_MODULE";
	res->status = RESULT::EStatus::success;
	res->type = RESULT::EType::executive;
	res->feedback = std::to_string(mask.first) + ":" + std::to_string(mask.second);
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " feedback: " << res->feedback;
	cachePtr_->addToChildren(mirrorModule_, res);
}

void MirrorModule::onDoorsLock()
{
	auto action = actionSet_->getAction("CLOSE_MIRROR");
	auto mask = createMaskForConnectorChange(action->connIds, action->connValues);
	BOOST_LOG(logger_) << "INF " << "MirrorModule::handleDoorsStateChange: mirrors closing";
	auto res = new RESULT();
	res->applicant = "MIRROR_MODULE";
	res->status = RESULT::EStatus::success;
	res->type = RESULT::EType::executive;
	res->feedback = std::to_string(mask.first) + ":" + std::to_string(mask.second);
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " feedback: " << res->feedback;
	cachePtr_->addToChildren(mirrorModule_, res);
}

std::pair<int, int> MirrorModule::createMaskForConnectorChange(std::vector<int> portsId, std::vector<int> portsValues)
{
	int sum = 0;
	int iterations = portsId.size();
	int decMask1, decMask2 = 0;
	for (int i = 0; i < iterations; i++)
	{
		if (portsValues[i] == 1)
		{
			sum += std::pow(2, portsId[i]);
		}
	}
	if (sum > 255)
	{
		decMask1 = 255;
		decMask2 = sum - decMask1;
	}
	else
	{
		decMask1 = sum;
		decMask2 = 0;
	}
	return std::make_pair(decMask1, decMask2);
}

void MirrorModule::getBDMModule()
{
	auto moduleVec = cachePtr_->getAllObjectsFromChildren("EQM", "MODULE");
	if (!moduleVec.empty())
	{
		BOOST_LOG(logger_) << "INF " << "MirrorModule::getBDMModules " << "EQM object found";
		for (const auto &mod : moduleVec)
		{
			if (static_cast<MODULE*>(mod)->label.find("BDM_MIRROR") != std::string::npos)
			{
				BOOST_LOG(logger_) << "INF " << "MirrorModule::getBDMModule: MODULE found";
				mirrorModule_ = static_cast<MODULE*>(mod);
				return;
			}
		}
	}
	BOOST_LOG(logger_) << "ERR " << "MirrorModule::getBDMModule: MODULE not found";
}

void MirrorModule::createMirrors()
{
	mirrorsObj_ = new MIRRORS();
	auto connsVec = cachePtr_->getAllObjectsUnder(mirrorModule_, "CONNECTOR");
	std::vector<std::string> mirrorsLabels;
	for (const auto &conn : connsVec)
	{
		auto connC = static_cast<CONNECTOR*>(conn);
		if (connC->label.find("LEFT") != std::string::npos || connC->label.find("RIGHT") != std::string::npos)
		{
			BOOST_LOG(logger_) << "INF " << "MirrorModule::createMirrors: label";
			std::vector<std::string> sLabel;
			boost::split(sLabel, connC->label, boost::is_any_of("_"));
			auto exist = [](std::vector<std::string> mirrLabels, std::string label)->bool
			{
				for (const auto &lab : mirrLabels)
				{
					if (label.find(lab) != std::string::npos)
						return true;
				}
				return false;
			}(mirrorsLabels, sLabel[0] + "_" + sLabel[1]);
			BOOST_LOG(logger_) << "DBG " << "MirrorModule::createMirrors: label: " << sLabel[0] + "_" + sLabel[1] << " exist " << exist;
			if (!exist)
				mirrorsLabels.push_back(sLabel[0] + "_" + sLabel[1]);
		}
	}
	for (const auto &label : mirrorsLabels)
	{
		BOOST_LOG(logger_) << "INF " << "MirrorModule::createMirrors " << "creating MIRROR " << label;
		auto mirror = new MIRROR();
		mirror->label = label;
		mirror->openingState = MIRROR::EOpeningState::closed;
		mirror->x = 0;
		mirror->y = 0;
		for (const auto &conn : connsVec)
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			if (connC->label.find(mirror->label) != std::string::npos)
			{
				mirror->refs.push_back(connC->id);
			}
		}
		mirrorsObj_->children.push_back(mirror);
	}
	if (BDM_DBG)
		displayTopology();
}

void MirrorModule::displayTopology()
{
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__;
	for (const auto &mirror : mirrorsObj_->children)
	{
		auto mirrorC = static_cast<MIRROR*>(mirror);
		BOOST_LOG(logger_) << "DBG " << "MIRROR " << mirrorC->label;
		BOOST_LOG(logger_) << "DBG " << "REFs:";
		for (const auto &ref : mirrorC->refs)
			BOOST_LOG(logger_) << "DBG " << ref;
	}
}

void MirrorModule::setMirrorPosition(int x, int y, int z, int label)
{
	
}

std::vector<int> MirrorModule::getPortsIdForMirrorClosing()
{
	int connleft1 = getConnIdByLabel("MIRROR_LEFT_MOTOR_OUT_1");
	int connleft2 = getConnIdByLabel("MIRROR_LEFT_MOTOR_OUT_2");
	int connright1 = getConnIdByLabel("MIRROR_RIGHT_MOTOR_OUT_1");
	int connright2 = getConnIdByLabel("MIRROR_RIGHT_MOTOR_OUT_2");
	return{ connleft1, connleft2, connright1, connright2 };
}

bool MirrorModule::checkIfMotorOutExist()
{
	for (const auto &connector : mirrorModule_->children)
	{
		if (connector->name == "CONNECTOR")
		{
			auto connC = static_cast<CONNECTOR*>(connector);
			if (connC->label.find("MOTOR_OUT") != std::string::npos)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " true";
				return true;
			}
		}
	}
	return false;
}

int MirrorModule::getConnIdByLabel(std::string label)
{
	for (const auto &conn : mirrorModule_->children)
	{
		auto connC = static_cast<CONNECTOR*>(conn);
		if (connC != nullptr && connC->label == label)
			return connC->id;
	}
}

