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
	doors_ = *static_cast<DOORS*>(cachePtr_->getUniqueObject("DOORS"));
	cachePtr_->subscribe("MODULE_TASK", std::bind(&MirrorModule::handleModuleTask, this, std::placeholders::_1), { 0 });
	if (checkIfMotorOutExist())
		doorsSubscId_ = cachePtr_->subscribe("DOORS", std::bind(&MirrorModule::handleDoorsStateChange, this, std::placeholders::_1), { 1 })[0];
}

void MirrorModule::handleModuleTask(Obj* obj)
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	auto moduleTask = static_cast<MODULE_TASK*>(obj);
	if (moduleTask->taskFor == mirrorModule_->domain && moduleTask->type == MODULE_TASK::EName::MASK_CONNECTORS_STATE)
	{
		auto mcs = static_cast<MASK_CONNECTORS_STATE*>(moduleTask);
		std::bitset<8> bMask1(mcs->mask1);
		std::bitset<8> bMask2(mcs->mask2);
		std::bitset<8> bMask3(mcs->mask3);
		std::bitset<8> bMask4(mcs->mask4);
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MASK_CONNECTORS_STATE: " << mcs->mask1 << " " << mcs->mask2;
		for (int i = 0; i < 8; i++)
		{
			auto sconn = static_cast<CONNECTOR*>(mirrorModule_->children[i]);
			if (sconn != nullptr && sconn->value != bMask1[i])
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " changeConnectorState: " << sconn->label << " to value " << bMask1[i];
				sconn->value = bMask1[i];
			}
		}
		if (mirrorModule_->children.size() > 8 && mirrorModule_->children.size() < 16)
		{
			for (int i = 8; i < mirrorModule_->children.size(); i++)
			{
				if (mirrorModule_->children[i]->name == "CONNECTOR")
				{
					auto sconn = static_cast<CONNECTOR*>(mirrorModule_->children[i]);
					if (sconn != nullptr && sconn->value != bMask2[i - 8])
					{
						BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " changeConnectorState: " << sconn->label << " to value " << bMask1[i - 8];
						sconn->value = bMask2[i - 8];
					}
				}
					
			}

		}
		if (mirrorModule_->children.size() > 16 && mirrorModule_->children.size() < 24)
		{
			for (int i = 16; i < mirrorModule_->children.size(); i++)
			{
				if (mirrorModule_->children[i]->name == "CONNECTOR")
				{
					auto sconn = static_cast<CONNECTOR*>(mirrorModule_->children[i]);
					if (sconn != nullptr && sconn->value != bMask3[i - 16])
					{
						BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " changeConnectorState: " << sconn->label << " to value " << bMask3[i - 16];
						sconn->value = bMask3[i - 16];
					}
				}

			}

		}
		if (mirrorModule_->children.size() > 24 && mirrorModule_->children.size() < 32)
		{
			for (int i = 24; i < mirrorModule_->children.size(); i++)
			{
				if (mirrorModule_->children[i]->name == "CONNECTOR")
				{
					auto sconn = static_cast<CONNECTOR*>(mirrorModule_->children[i]);
					if (sconn != nullptr && sconn->value != bMask4[i - 24])
					{
						BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " changeConnectorState: " << sconn->label << " to value " << bMask4[i - 24];
						sconn->value = bMask4[i - 24];
					}
				}

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
			auto action = actionSet_->getAction("OPEN_MIRROR");
			auto mask = createMaskForConnectorChange(action->connIds, action->connValues);
			BOOST_LOG(logger_) << "INF " << "MirrorModule::handleDoorsStateChange: mirrors opening";
			auto res = new RESULT();
			res->applicant = "MIRROR_MODULE";
			//createMaskForConnector({}
			res->feedback = std::to_string(mask.first) + ":" + std::to_string(mask.second);
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " feedback: " << res->feedback;
			res->status = RESULT::EStatus::success;
			res->type = RESULT::EType::executive;
			cachePtr_->addToChildren(mirrorModule_, res);
		}
		if (doors_.lockingState == DOORS::ELockingState::locked
			&& mirrorsObj_->openingState != MIRRORS::EOpeningState::opened)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MIRROS changing state to: readyToClose";
			mirrorsObj_->openingState = MIRRORS::EOpeningState::readyToClose;
		}
		//SHOULD BE ADDED FOR CLOSING BUT WE NEED TO CHANGE POLARISATION OF MOTOR
	}
	else if (doors_.lockingState == newDoors->lockingState &&
		mirrorsObj_->openingState == MIRRORS::EOpeningState::readyToClose)
	{
		auto action = actionSet_->getAction("CLOSE_MIRROR");
		auto mask = createMaskForConnectorChange(action->connIds, action->connValues);
		BOOST_LOG(logger_) << "INF " << "MirrorModule::handleDoorsStateChange: mirrors closing";
		auto res = new RESULT();
		res->applicant = "MIRROR_MODULE";
		//createMaskForConnector({}
		res->feedback = std::to_string(mask.first) + ":" + std::to_string(mask.second);
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " feedback: " << res->feedback;
		res->status = RESULT::EStatus::success;
		res->type = RESULT::EType::executive;
		cachePtr_->addToChildren(mirrorModule_, res);
	}
}

std::pair<int, int> MirrorModule::createMaskForConnectorChange(std::vector<int> portsId, std::vector<int> portsValues)
{
	int decMask1 = 0;
	int decMask2 = 0;
	if (mirrorModule_->children.size() > 8)
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " more than 8 connctors";
		for (int i = 0; i < 8; i++)
		{
			auto exist = [](std::vector<int> ids, int i)->int
			{
				int j = 0;
				for (const auto &id : ids)
				{
					if (id == i)
						return j;
					j++;
				}
				return -1;
			}(portsId, i);
			if (exist != -1)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " first8: connector will be changed " << i << " portsValue " << portsValues[exist];
				decMask1 += portsValues[exist] * static_cast<int>(pow(2, i));
			}
			else
				decMask1 += static_cast<CONNECTOR*>(mirrorModule_->children[i])->value * static_cast<int>(pow(2, i));
		}
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " mask1: " << decMask1;
		auto conns = cachePtr_->getAllObjectsUnder(mirrorModule_, "CONNECTOR");
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connsize: " << conns.size();
		for (int i = 8; i < conns.size(); i++)
		{
			auto exist = [](std::vector<int> ids, int i)->int
			{
				int j = 0;
				for (const auto &id : ids)
				{
					if (id == i)
						return j;
					j++;
				}
				return -1;
			}(portsId, i);
			if (exist != -1)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " second8: connector will be changed " << i;
				decMask2 += portsValues[exist] * static_cast<int>(pow(2, i));
			}
			else
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " second8: connector will be not changed " << i;
				decMask2 += static_cast<CONNECTOR*>(conns[i])->value * static_cast<int>(pow(2, i));
			}
				
		}
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " mask2: " << decMask2;
	}
	else
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " less than 8 connns";
		decMask1 = 0;
		auto conns = cachePtr_->getAllObjectsUnder(mirrorModule_, "CONNECTOR");
		for (int i = 8; i < conns.size(); i++)
		{
			auto exist = [](std::vector<int> ids, int i)->int
			{
				int j = 0;
				for (const auto &id : ids)
				{
					if (id == i)
						return j;
					j++;
				}
				return -1;
			}(portsId, i);
			if (exist != -1)
			{
				static_cast<CONNECTOR*>(mirrorModule_->children[i])->value = portsValues[exist];
			}
			decMask2 += static_cast<CONNECTOR*>(mirrorModule_->children[i])->value * (pow(2, i));
		}
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " mask1: " << decMask1;
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " mask2: " << decMask2;

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
	//BOOST_LOG(logger_) << "DBG " << "MirrorModule::displayTopology " << "commonOutGnd " << mirrorsObj_->commonOutGND->label;
	for (const auto &mirror : mirrorsObj_->children)
	{
		auto mirrorC = static_cast<MIRROR*>(mirror);
		BOOST_LOG(logger_) << "DBG " << "MirrorModule::displayTopology " << "MIRROR " << mirrorC->label;
		for (const auto &conn : mirror->children)
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			BOOST_LOG(logger_) << "DBG " << "MirrorModule::displayTopology " << "CONNECTOR " << connC->id << " label " << connC->label << " value " << connC->value;
		}
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

int MirrorModule::getConnIdByLabel(std::string label)
{
	for (const auto &conn : mirrorModule_->children)
	{
		auto connC = static_cast<CONNECTOR*>(conn);
		if (connC != nullptr && connC->label == label)
			return connC->id;
	}
}

