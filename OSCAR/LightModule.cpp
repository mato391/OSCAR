#include "stdafx.h"
#include "LightModule.h"


LightModule::LightModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr)
{
	cachePtr_ = cachePtr;
	logger_ = logger;
	eLA_ = new EmergencyLightsAgent(logger, cachePtr);
	BOOST_LOG(logger_) << "DBG " << "LightModule ctor";
}

LightModule::~LightModule()
{
}

void LightModule::setup()
{
	bdmModuleObj_->protocol = MODULE::EProtocol::CExtendedMessage;
	cmdiSubscrId_ = cachePtr_->subscribe("CONNECTORS_MASKING_DONE_IND", std::bind(&LightModule::handleIndication, this, std::placeholders::_1), { 0 })[0];
	doorsChangeSubscId_ = cachePtr_->subscribe("DOORS", std::bind(&LightModule::handleDoorsStateChange, this, std::placeholders::_1), { 0, 1 }); //subscribe for create
	eLA_->getBlinkers();
}

void LightModule::initialize()
{
	BOOST_LOG(logger_) << "INF " << "LightModule::initialize";
	getBDMModules();
	createLightsTopology();
	//bLA_ = new BeamLightAgent(logger_, cachePtr_);
	//displayTopology();
}

void LightModule::getBDMModules()
{
	auto moduleVec = cachePtr_->getAllObjectsFromChildren("EQM", "MODULE");
	if (!moduleVec.empty())
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << "EQM object found";
		for (const auto &mod : moduleVec)
		{
			if (static_cast<MODULE*>(mod)->label.find("BDM_LIGHT") != std::string::npos)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MODULE found";
				bdmModuleObj_ = static_cast<MODULE*>(mod);
				return;
			}
		}
	}
	BOOST_LOG(logger_) << "ERR " << "DoorModule::getBDMModule: MODULE not found";
}

void LightModule::createLightsObj()
{
	lightes_ = new LIGHTES();
	cachePtr_->addToChildren(bdmModuleObj_, lightes_);
}

void LightModule::createLightsTopology()
{
	BOOST_LOG(logger_) << "INF " << "LightModule::createLightsTopology";
	std::vector<std::string> labels;
	connsVec_ = cachePtr_->getAllObjectsUnder(bdmModuleObj_, "CONNECTOR");
	for (auto &conn : connsVec_)
	{
		auto connC = static_cast<CONNECTOR*>(conn);
		//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " " << connC->label;
		std::vector<std::string> sconnLabel;
		boost::split(sconnLabel, connC->label, boost::is_any_of("_"));
		bool isAlready = [](std::vector<std::string>* labelsVec, std::string connLab)->bool
		{
			if (labelsVec->empty())
				return false;
			for (const auto &label : *labelsVec)
			{
				if (label == connLab)
					return true;
			}
			return false;
		}(&labels, sconnLabel[1] + "_" + sconnLabel[2]);
		if (!isAlready
			&& connC->label.find("GND") == std::string::npos)
		{
			BOOST_LOG(logger_) << "INF " << "LightModule::createLightsTopology adding label: " << connC->label;
			labels.push_back(connC->label);
		}
		//else
		//	BOOST_LOG(logger_) << "DBG " << "LightModule::createLightsTopology is already added: " << connC->label;

	}
	createLightsObj();
	createLightObjs(labels);
	cachePtr_->addToChildren(bdmModuleObj_, lightes_);
}

void LightModule::createLightObjs(std::vector<std::string> labels)
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " labels.size: " << labels.size();
	for (const auto &label : labels)
	{
		auto light = new LIGHT(label);
		for (const auto &conn : connsVec_)
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			std::vector <std::string> slabel;
			boost::split(slabel, light->label, boost::is_any_of("_"));
			//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connLabel: " << connC->label << " label " << label << " shortcutedConnCLabel " <<
			slabel[0] + "_" + slabel[1] + "_" + slabel[2];
			if (connC->label.find(label) != std::string::npos
				|| (connC->label.find("GND") != std::string::npos && connC->label.find(slabel[0] + "_" + slabel[1] + "_" + slabel[2]) != std::string::npos)
				|| (connC->label.find("BLINKER") != std::string::npos && connC->label.find("GND") != std::string::npos && connC->label.find(slabel[0] + "_" + slabel[1]) != std::string::npos))
			{
				light->refs.push_back(connC->id);
			}
		}
		lightes_->children.push_back(light);
	}
	displayTopology();

}

void LightModule::displayTopology()
{
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__;
	for (const auto &obj : lightes_->children)
	{
		auto light = static_cast<LIGHT*>(obj);
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " LIGHT " << light->label;
		for (const auto &ref : light->refs)
			BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " REF " << ref;
	}
}

void LightModule::handleIndication(Obj* obj)
{
	auto cmdInd = static_cast<CONNECTORS_MASKING_DONE_IND*>(obj);
	if (cmdInd->domain != bdmModuleObj_->domain)
		return;
	for (const auto &conn : cmdInd->connectors_)
	{
		if (conn->name == "CONNECTOR")
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			std::vector<std::string> sLabel;
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " " << connC->label;
			setLightState(connC->value, connC->id);
		}
		
	}

}

void LightModule::setLightState(int connValue, int connId)
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " for connector " << connId << " to value " << connValue;
	for (const auto &obj : lightes_->children)
	{
		auto light = static_cast<LIGHT*>(obj);
		for (const auto &ref : light->refs)
		{
			if (ref == connId)
			{
				setLightStateBasedOnConnId(light, connId, connValue);
			}
		}
	}
}

void LightModule::setLightStateBasedOnConnId(LIGHT* light, int connId, int connValue)
{
	auto connVec = cachePtr_->getAllObjectsUnder(bdmModuleObj_, "CONNECTOR");
	for (const auto &obj : connVec)
	{
		auto conn = static_cast<CONNECTOR*>(obj);
		if (conn->id == connId
			&& conn->label.find("GND") != std::string::npos
			&& connValue == 0)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " LIGHT: " << light->label << "turning on";
			light->proceduralState = LIGHT::EProceduralState::on;
		}
		else if (conn->id == connId
			&& conn->label.find("GND") != std::string::npos
			&& connValue == 1)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " LIGHT: " << light->label << "turning off";
			light->proceduralState = LIGHT::EProceduralState::off;
		}

	}
}

void LightModule::handleDoorsStateChange(Obj* obj)
{
	if (doorsObj_.children.empty())
	{
		doorsObj_ = *(static_cast<DOORS*>(obj));
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " Doors object has no door";
	}
	else
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " Doors object has door";
		
	}
	compareStates(obj);
}

void LightModule::compareStates(Obj* obj)
{
	auto doorsObj = static_cast<DOORS*>(obj);
	auto res = new RESULT();
	res->applicant = "ELA";
	res->feedback = "";
	res->status = RESULT::EStatus::success;
	res->type = RESULT::EType::executive;
	auto masks = eLA_->createMask();
	if (doorsObj != nullptr && doorsObj_.openingState != doorsObj->openingState)
	{
		BOOST_LOG(logger_) << "INF " << "LightModule::compareStates " << "openingState has been changed";
	}
	else if (doorsObj != nullptr && doorsObj_.lockingState != doorsObj->lockingState)
	{
		doorsObj_.lockingState = doorsObj->lockingState;
		BOOST_LOG(logger_) << "INF " << "LightModule::compareStates " << "lockingState has been changed";
		for (const auto &mask : masks)
			res->feedback += std::to_string(mask) + ":";
		if (doorsObj_.lockingState == DOORS::ELockingState::unlocked)
			res->feedback += "2:5";
		else
			res->feedback += "1:5";
		cachePtr_->addToChildren(bdmModuleObj_, res);
	}
	else
	{
		for (const auto &mask : masks)
			res->feedback += std::to_string(mask) + ":";
		if (doorsObj_.openingState == DOORS::EOpeningState::closed
			&& doorsObj_.lockingState == DOORS::ELockingState::unlocked)
			res->feedback += "2:5";
		else if (doorsObj_.openingState == DOORS::EOpeningState::closed
			&& doorsObj_.lockingState == DOORS::ELockingState::locked)
			res->feedback += "1:5";
		cachePtr_->addToChildren(bdmModuleObj_, res);
	}
}


