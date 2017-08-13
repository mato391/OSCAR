#include "stdafx.h"
#include "UCM.hpp"


UCM::UCM(std::string domain, boost::log::sources::logger_mt logger)
{
	this->name = "UCM";
	this->domain = domain;
	this->logger_ = logger;
	BOOST_LOG(logger_) << "DEBUG " << "UCM ctor";
	
}

UCM::~UCM()
{
}

void UCM::initialize()
{
	getUCMModule();
	prepareSwitchTopology();
	startStopEngineButtonAgent_ = new StartStopEngineButtonAgent(switchTopology, logger_);
	ucmModuleObj_->protocol = MODULE::EProtocol::CSimpleMessage;	
}

RESULT* UCM::setup(int domain)
{
	ccdiSubscrId_ = cachePtr->subscribe("CHANGE_CONNECTOR_DONE_IND", std::bind(&UCM::handleConnectorChange, this, std::placeholders::_1), { 0 })[0];
	RESULT* result = new RESULT();
	result->status = RESULT::EStatus::success;
	result->feedback = std::to_string(static_cast<int>(ucmModuleObj_->protocol));
	return result;
}

void UCM::handleConnectorChange(Obj* obj)
{
	auto ccdi = static_cast<CHANGE_CONNECTOR_DONE_IND*>(obj);
	if (ccdi->connector->label.find("EMCY") != std::string::npos)
	{
		CHANGE_BUTTON_STATE_IND* cbsi = new CHANGE_BUTTON_STATE_IND();
		cbsi->buttonLabel = "EMCY_BUTTON_1";
		cbsi->value = ccdi->connector->value;
		cachePtr->addToChildren(ucmModuleObj_, cbsi);
	}
}

void UCM::getUCMModule()
{
	auto moduleVec = cachePtr->getAllObjectsFromChildren("EQM", "MODULE");
	if (!moduleVec.empty())
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " EQM object found";
		for (const auto &mod : moduleVec)
		{
			if (static_cast<MODULE*>(mod)->label.find("UCM") != std::string::npos)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " MODULE found";
				ucmModuleObj_ = static_cast<MODULE*>(mod);
				return;
			}
		}
	}
	BOOST_LOG(logger_) << "ERR " << __FUNCTION__ << " MODULE not found";
}

void UCM::prepareSwitchTopology()
{
	switchTopology = new SWITCH_TOPOLOGY();
	std::vector<std::string> buttonLabels;
	auto connVec = cachePtr->getAllObjectsUnder(ucmModuleObj_, "CONNECTOR");
	for (const auto &conn : connVec)
	{
		auto connC = static_cast<CONNECTOR*>(conn);
		std::vector<std::string> splittedLabel;
		boost::split(splittedLabel, connC->label, boost::is_any_of("_"));
		auto exist = [](std::vector<std::string> currLabels, std::string connLabel)->bool
		{
			std::vector<std::string> splittedLabel;
		    boost::split(splittedLabel, connLabel, boost::is_any_of("_"));
			for (const auto &label : currLabels)
			{
				if (label == splittedLabel[0] + "_" + splittedLabel[1])
				{
					return true;
				}
			}
			return false;
		}(buttonLabels, splittedLabel[0] + "_" + splittedLabel[1]);
		if (!exist)
		{
			buttonLabels.push_back(splittedLabel[0] + "_" + splittedLabel[1]);
		}
		
	}
	for (const auto &label : buttonLabels)
	{
		BUTTON* button = new BUTTON();
		button->label = label;
		for (const auto &conn : connVec)
		{
			auto connC = static_cast<CONNECTOR*>(conn);
			if (connC->label.find(button->label) != std::string::npos)
			{
				button->refs.push_back(connC->id);
			}
		}
		switchTopology->children.push_back(button);
	}
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " created: " << switchTopology->children.size() << " buttons";
	cachePtr->addToChildren(ucmModuleObj_, switchTopology);
}


void UCM::displayTopology()
{
	
}

void UCM::execute(std::string message)
{

}

CMESSAGE::CMessage* UCM::execute(CMESSAGE::CMessage* msg)
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " " << msg->header;
	if (msg->getProtocol() == CMESSAGE::CMessage::EProtocol::CInitialProtocol
		&& msg->header == AA)
	{
		CMESSAGE::CInitialMessage* mesg = new CMESSAGE::CInitialMessage();
		mesg->header = AA;
		mesg->protocol = CMESSAGE::CMessage::EProtocol::CInitialProtocol;
		mesg->fromDomain = 100;
		mesg->toDomain = msg->fromDomain.substr(2, 2);
		mesg->optional1 = 0;
		mesg->optional2 = 0;
		return mesg;
	}
	else if (msg->getProtocol() == CMESSAGE::CMessage::EProtocol::CSimpleProtocol)
	{
		auto msgC = static_cast<CMESSAGE::CSimpleMessage*>(msg);
		CHANGE_BUTTON_STATE_IND* cbsi = new CHANGE_BUTTON_STATE_IND();
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " CHANGE_BUTTON_STATE_IND port: " << msgC->port << " value: " << msgC->value;
		for (const auto &conn : ucmModuleObj_->children)
		{
			if (conn->name == "CONNECTOR")
			{
				auto connC = static_cast<CONNECTOR*>(conn);
				if (connC->id == msgC->port)
				{
					std::vector<std::string> splittedLabel;
					boost::split(splittedLabel, connC->label, boost::is_any_of("_"));
					connC->value = msgC->value;
					cbsi->buttonLabel = splittedLabel[0] + "_" + splittedLabel[1];
					cbsi->value = msgC->value;
				}
					
			}
		}
		for (auto &button : switchTopology->children)
		{
			auto buttonC = static_cast<BUTTON*>(button);
			if (cbsi->buttonLabel.find(buttonC->label) != std::string::npos)
			{
				//buttonC->turnOnLed();
				buttonC->value = cbsi->value;
			}
		}
		cachePtr->addToChildren(ucmModuleObj_, cbsi);
	}
	return nullptr;
}

void UCM::execute(INTER_MODULE_OPERATION* imo)
{

}


