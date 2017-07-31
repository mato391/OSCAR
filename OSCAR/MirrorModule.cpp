#include "stdafx.h"
#include "MirrorModule.hpp"
#define DEBUG true

MirrorModule::MirrorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr)
{
	cache_ = cache;
	cachePtr_ = cachePtr;
	logger_ = logger;
	//createMirrors();
}


MirrorModule::~MirrorModule()
{
}

void MirrorModule::initialize()
{
	getBDMModule();
	createMirrors();
}

void MirrorModule::setup()
{
	BOOST_LOG(logger_) << "INF " << "MirrorModule::setup " << std::endl;
	mirrorModule_->protocol = MODULE::EProtocol::CExtendedMessage;
	mirrorModule_->operationalState = MODULE::EOperationalState::enabled;
	doors_ = *static_cast<DOORS*>(cachePtr_->getUniqueObject("DOORS"));
	cachePtr_->subscribe("MODULE_TASK", std::bind(&MirrorModule::handleModuleTask, this, std::placeholders::_1), { 0 });
	if (checkIfMotorOutExist())
		int doorsSubscId_ = cachePtr_->subscribe("DOORS", std::bind(&MirrorModule::handleDoorsStateChange, this, std::placeholders::_1), { 1 })[0];
}
void MirrorModule::handleModuleTask(Obj* obj)
{
	auto moduleTask = static_cast<MODULE_TASK*>(obj);
	if (moduleTask->type == MODULE_TASK::EName::CHANGE_CONNECTOR_STATE_TASK)
	{
		auto ccst = static_cast<CHANGE_CONNECTOR_STATE_TASK*>(moduleTask);
		for (const auto &conn : mirrorModule_->children)
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			if (connC->id == ccst->port)
			{
				BOOST_LOG(logger_) << "INF " << "MirrorModule::handleModuleTask: changeConnectorState: " << ccst->port << " to value " << ccst->value;
				connC->value = ccst->value;
				return;
			}
		}
		if (mirrorsObj_->commonOutGND->id == ccst->port)
		{
			mirrorsObj_->commonOutGND->value = ccst->value;
			if (mirrorsObj_->openingState == MIRRORS::EOpeningState::closed
				&& ccst->value == 0)
			{
				mirrorsObj_->openingState = MIRRORS::EOpeningState::closed;
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MIRRORS changing state to closed";
			}
			else if (mirrorsObj_->openingState == MIRRORS::EOpeningState::opened
				&& ccst->value == 0)
			{
				mirrorsObj_->openingState = MIRRORS::EOpeningState::opened;
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MIRRORS changing state to opened";
			}
			else if (mirrorsObj_->openingState == MIRRORS::EOpeningState::opened
				&& ccst->value == 1)
			{
				mirrorsObj_->openingState = MIRRORS::EOpeningState::closing;
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MIRRORS changing state to closing";
			}
			else if (mirrorsObj_->openingState == MIRRORS::EOpeningState::closed
				&& ccst->value == 1)
			{
				mirrorsObj_->openingState = MIRRORS::EOpeningState::opening;
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MIRRORS changing state to opening";
			}
		}	
	}
}

void MirrorModule::handleDoorsStateChange(Obj* obj)
{
	BOOST_LOG(logger_) << "INF " << "MirrorModule::handleDoorsStateChange";
	auto newDoors = static_cast<DOORS*>(obj);
	if (doors_.lockingState != newDoors->lockingState)
	{
		doors_.lockingState = newDoors->lockingState;
		if (doors_.lockingState == DOORS::ELockingState::unlocked
			&& mirrorsObj_->openingState != MIRRORS::EOpeningState::opened)
		{
			BOOST_LOG(logger_) << "INF " << "MirrorModule::handleDoorsStateChange: mirrors opening";
			auto res = new RESULT();
			res->applicant = "MIRROR_MODULE";
			res->feedback = std::to_string(mirrorsObj_->commonOutGND->id)  + ":1:125";
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " feedback: " << res->feedback;
			res->status = RESULT::EStatus::success;
			res->type = RESULT::EType::executive;
			cachePtr_->addToChildren(mirrorModule_, res);
		}
	}
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
		else if (connC->label.find("OUT_GND") != std::string::npos)
		{
			mirrorsObj_->commonOutGND = connC;
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
		mirrorsObj_->children.push_back(mirror);
	}
	for (const auto &mirror : mirrorsObj_->children)
	{
		auto mirrorC = static_cast<MIRROR*>(mirror);
		for (const auto &conn : connsVec)
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			if (connC->label.find(mirrorC->label) != std::string::npos)
			{
				mirrorC->children.push_back(connC);
			}
		}
	}
	if (BDM_DBG)
		displayTopology();
}

void MirrorModule::displayTopology()
{
	BOOST_LOG(logger_) << "DBG " << "MirrorModule::displayTopology " << "commonOutGnd " << mirrorsObj_->commonOutGND->label;
	for (const auto &mirror : mirrorsObj_->children)
	{
		auto mirrorC = static_cast<MIRROR*>(mirror);
		BOOST_LOG(logger_) << "DBG " << "MirrorModule::displayTopology " << "MIRROR " << mirrorC->label;
		for (const auto &conn : mirror->children)
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			BOOST_LOG(logger_) << "DBG " << "MirrorModule::displayTopology " << "CONNECTOR " << connC->id << " label " << connC->label;
		}
	}
}

void MirrorModule::setMirrorPosition(int x, int y, int z, int label)
{
	
}

bool MirrorModule::checkIfMotorOutExist()
{
	for (const auto &mirror : mirrorsObj_->children)
	{
		for (const auto &conn : mirror->children)
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			if (connC->label.find("MOTOR_OUT") != std::string::npos)
			{
				return true;
			}
		}
	}
	return false;
}

