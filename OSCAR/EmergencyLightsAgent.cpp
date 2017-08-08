#include "stdafx.h"
#include "EmergencyLightsAgent.hpp"


EmergencyLightsAgent::EmergencyLightsAgent(boost::log::sources::logger_mt logger, Cache* cachePtr) : cachePtr_(cachePtr), logger_(logger)
{
	
}


EmergencyLightsAgent::~EmergencyLightsAgent()
{

}

void EmergencyLightsAgent::getBlinkers()
{
	auto moduleVec = cachePtr_->getAllObjectsFromChildren("EQM", "MODULE");
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " moduleSize " << moduleVec.size();
	for (const auto &module : moduleVec)
	{
		auto moduleC = static_cast<MODULE*>(module);
		if (moduleC->label == "BDM_LIGHT")
		{
			lightModuleObjPtr_ = static_cast<MODULE*>(moduleC);
		}
	}
	if (lightModuleObjPtr_ == nullptr)
		BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " no LIGHTES object found";
	else
	{
		for (const auto &conn : lightModuleObjPtr_->children)
		{
			if (conn->name == "CONNECTOR")
			{
				auto connC = static_cast<CONNECTOR*>(conn);
				if (connC->label.find("LIGHT_BLINKERRIGHT_RIGHT_GND") != std::string::npos
					|| connC->label.find("LIGHT_BLINKERLEFT_LEFT_GND") != std::string::npos)
				{
					blinkersCommonConns_.push_back(connC);
				}
			}
		}
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " blinkersCommonConns: " << blinkersCommonConns_.size();
		cachePtr_->subscribe("CHANGE_BUTTON_STATE_IND", std::bind(&EmergencyLightsAgent::handleChangeButtonStateIndication, this, std::placeholders::_1), { 0 });
	}
}

void EmergencyLightsAgent::handleChangeButtonStateIndication(Obj* obj)
{
	auto cbsi = static_cast<CHANGE_BUTTON_STATE_IND*>(obj);
	if (cbsi->buttonLabel == "EMCY_BUTTON")
	{
		bssf_ = true;
		boost::thread t(std::bind(&EmergencyLightsAgent::startBlinkerService, this));
		t.detach();
	}
}

void EmergencyLightsAgent::startBlinkerService()
{
	auto mask = createMask();
	RESULT * result = new RESULT();
	result->applicant = "ELA";
	result->type = RESULT::EType::executive;
	result->feedback = std::to_string(mask.first) + ":" + std::to_string(mask.second) + ":9:5";
	result->status = RESULT::EStatus::success;
	cachePtr_->addToChildren(lightModuleObjPtr_, result);
	result = new RESULT();
}

std::pair<int, int> EmergencyLightsAgent::createMask()
{
	int decMask1 = 0;
	int decMask2 = 0;
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " more than 8 connctors";
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " blinkersCommonConns_ " << blinkersCommonConns_.size();
	for (int i = 0; i < lightModuleObjPtr_->children.size(); i++)
	{
		if (lightModuleObjPtr_->children[i]->name == "CONNECTOR")
		{
			auto connC = static_cast<CONNECTOR*>(lightModuleObjPtr_->children[i]);
			if (connC->id < 8 
				&& (connC->label == blinkersCommonConns_[0]->label 
				|| connC->label == blinkersCommonConns_[1]->label))
			{
				decMask1 += static_cast<int>(pow(2, i));
				BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask1: " << decMask1;
				BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask2: " << decMask2;
			}
			else if (connC->id > 8
				&& (connC->label == blinkersCommonConns_[0]->label
					|| connC->label == blinkersCommonConns_[1]->label))
			{
				BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connC->id >>> 8 connC->label is blinker";
				decMask2 += static_cast<int>(pow(2, i - 8));
				BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask1: " << decMask1;
				BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask2: " << decMask2;
			}
		}
	}
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask1: " << decMask1;
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask2: " << decMask2;
	return std::make_pair(decMask1, decMask2);
}