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
	if (cbsi->buttonLabel == "EMCY_BUTTON" && cbsi->value == 1)
	{
		bssf_ = true;
		boost::thread t(std::bind(&EmergencyLightsAgent::startBlinkerService, this));
		t.detach();
	}
	else if (cbsi->buttonLabel == "EMCY_BUTTON" && cbsi->value == 0)
	{
		boost::thread t(std::bind(&EmergencyLightsAgent::stopBlinkerService, this));
		t.detach();
	}
}

void EmergencyLightsAgent::stopBlinkerService()
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	auto masks = createMask();
	RESULT * result = new RESULT();
	result->applicant = "ELA";
	result->type = RESULT::EType::executive;
	result->feedback = "";
	for (const auto &mask : masks)
		result->feedback += std::to_string(mask) + ":";
	result->feedback += "0:0";
	result->status = RESULT::EStatus::success;
	cachePtr_->addToChildren(lightModuleObjPtr_, result);
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	if (blinkersCommonConns_[0]->value == 1 && blinkersCommonConns_[1]->value == 1)
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " clearing blinkers";
		auto masks = createMask();
		RESULT * result = new RESULT();
		result->applicant = "ELA";
		result->type = RESULT::EType::executive;
		result->feedback = "";
		for (const auto &mask : masks)
			result->feedback += std::to_string(mask) + ":";
		result->feedback += "0:0";
		result->status = RESULT::EStatus::success;
		cachePtr_->addToChildren(lightModuleObjPtr_, result);
	}
	
}

void EmergencyLightsAgent::startBlinkerService()
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	auto masks = createMask();
	RESULT * result = new RESULT();
	result->applicant = "ELA";
	result->type = RESULT::EType::executive;
	result->feedback = "";
	for (const auto &mask : masks)
		result->feedback += std::to_string(mask) + ":";
	result->feedback += "9:5";
	result->status = RESULT::EStatus::success;
	cachePtr_->addToChildren(lightModuleObjPtr_, result);
}

std::vector<int> EmergencyLightsAgent::createMask()
{
	int decMask1 = 0;
	int decMask2 = 0;
	int decMask3 = 0;
	int decMask4 = 0;
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " blinkersCommonConns_ " << blinkersCommonConns_.size();
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
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask1: " << decMask1;
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask2: " << decMask2;
			}
			else if (connC->id > 8
				&& (connC->label == blinkersCommonConns_[0]->label
					|| connC->label == blinkersCommonConns_[1]->label))
			{
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connC->id >>> 8 connC->label is blinker";
				decMask2 += static_cast<int>(pow(2, i - 8));
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask1: " << decMask1;
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask2: " << decMask2;
			}
			else if (connC->id > 16
				&& (connC->label == blinkersCommonConns_[0]->label
					|| connC->label == blinkersCommonConns_[1]->label))
			{
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connC->id >>> 8 connC->label is blinker";
				decMask3 += static_cast<int>(pow(2, i - 16));
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask1: " << decMask1;
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask2: " << decMask2;
			}
			else if (connC->id > 24
				&& (connC->label == blinkersCommonConns_[0]->label
					|| connC->label == blinkersCommonConns_[1]->label))
			{
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " connC->id >>> 8 connC->label is blinker";
				decMask4 += static_cast<int>(pow(2, i - 24));
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask1: " << decMask1;
				//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " decMask2: " << decMask2;
			}
		}
	}
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " decMask1: " << decMask1;
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " decMask2: " << decMask2;
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " decMask3: " << decMask3;
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " decMask4: " << decMask4;
	return{ decMask1 * 16, decMask2 * 16, decMask3 * 16, decMask4 * 16 };
}