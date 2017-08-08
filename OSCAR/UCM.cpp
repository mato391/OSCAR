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
	RESULT* result = new RESULT();
	result->status = RESULT::EStatus::success;
	result->feedback = std::to_string(static_cast<int>(ucmModuleObj_->protocol));
	return result;
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
	/*std::fstream scfFile("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\SCF.txt", std::ios::in);
	std::string scfContain;
	scfFile >> scfContain;
	scfFile.close();
	boost::split(scfVec_, scfContain, boost::is_any_of(";"));
	scfObj_ = new SCF();
	scfObj_->scfPath = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\SCF.txt";
	prepareTopologyBasedOnSCF();*/
	std::vector<std::string> buttonLabels;
	for (const auto &conn : ucmModuleObj_->children)
	{
		if (conn->name == "CONNECTOR")
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
	}
	for (const auto &label : buttonLabels)
	{
		BUTTON* button = new BUTTON();
		button->label = label;
		switchTopology->children.push_back(button);
	}
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " created: " << switchTopology->children.size() << " buttons";
	cachePtr->addToChildren(ucmModuleObj_, switchTopology);
}

void UCM::prepareTopologyBasedOnSCF()
{
	
}

void UCM::displayTopology()
{
	BOOST_LOG(logger_) << "DEBUG " << "UCM::displayTopology: display button topology";
	for (const auto &obj : switchTopology->buttonTopology)
	{
		BOOST_LOG(logger_) << "DEBUG " << "BUTTON: " << obj->label;
	}
	BOOST_LOG(logger_) << "DEBUG " << "UCM::displayTopology: display pedals topology";
	for (const auto &obj : switchTopology->pedals)
	{
		BOOST_LOG(logger_) << "DEBUG " << "PEDAL: " << obj->label;
	}
}

void UCM::execute(std::string message)
{
	//0x02yyzppp; yy - port, z - operation (position), p - percent (percent only for port > 01 and port < 05)
	std::string port = message.substr(4, 2);
	std::string percentage;
	std::string operation;
	if (message.size() > 7)
	{
		percentage = message.substr(7, message.size() - 7);
		BOOST_LOG(logger_) << "DBG " << percentage;
	}
	operation = message.substr(6, 1);	
	for (const auto & button : switchTopology->buttonTopology)
	{
		if (port == button->port)
		{
			startUserOperationProcessing(button);
			return;
		}
	}
	for (const auto &pedal : switchTopology->pedals)
	{
		if (port == pedal->port)
		{
			pedal->percent = std::stoi(percentage);
			BOOST_LOG(logger_) << "DBG " << "PEDAL label: " << port << " pedal percent " << pedal->percent;
			BOOST_LOG(logger_) << "DBG " << "Operation " << std::stoi(operation);
			pedal->isPushed = static_cast<bool>(std::stoi(operation));	
			startPedalOperation(pedal);		
			return;
		}
	}

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

void UCM::startUserOperationProcessing(BUTTON* button)
{
	RESULT result;
	if (button->label == "START_STOP_BUTTON")
	{
		result = startStopEngineButtonAgent_->buttonPushedAction();		
		if (result.status == RESULT::EStatus::success)
			getComponent("EDM")->execute(new INTER_MODULE_OPERATION("START_STOP_ENGINE", "1"));
	}
	
}

void UCM::startPedalOperation(PEDAL* pedal)
{
	if (pedal->label == "PEDAL_ACCEL")
	{
		getComponent("EDM")->execute(new INTER_MODULE_OPERATION("ACCELERATE_PERCENT_CHANGE", std::to_string(pedal->percent)));
		return;
	}
	for (auto &obj : *cache_)
	{
		if (obj->name == "ENGINE")
		{
			engineObjPtr_ = static_cast<ENGINE*>(obj);
		}
	}
	if (pedal->isPushed)
	{
		BOOST_LOG(logger_) << "INFO " << "UCM::startPedalOperation: clutching engine";
		getComponent("EDM")->execute(new INTER_MODULE_OPERATION("CLUTCH_ENGINE", "1"));
		//engineObjPtr_->operationalState = ENGINE::EOperationalState::clutched;
	}
	else if (!pedal->isPushed)
	{
		BOOST_LOG(logger_) << "INFO " << "UCM::startPedalOperation: unclutching engine";
		getComponent("EDM")->execute(new INTER_MODULE_OPERATION("UNCLUTCH_ENGINE", "0"));
		//engineObjPtr_->operationalState = ENGINE::EOperationalState::notClutched;
	}
}

int UCM::checkPreconditionToStartEngine()
{
	if (getPedalFromTopology("PEDAL_STOP")->isPushed && getPedalFromTopology("PEDAL_CLUTH")->isPushed)
		return 2;
	else if (getPedalFromTopology("PEDAL_STOP")->isPushed && !getPedalFromTopology("PEDAL_CLUTH")->isPushed)
		return 1;
	else
		return 0;
}

PEDAL* UCM::getPedalFromTopology(std::string label)
{
	for (const auto &pedal : switchTopology->pedals)
	{
		if (pedal->label == label)
			return pedal;
	}
	return nullptr;
}

Component* UCM::getComponent(std::string label)
{
	//BOOST_LOG(logger_) << "DBG " << "UCM::getComponent " << label;
	for (const auto &component : *componentCache_)
	{
		//BOOST_LOG(logger_) << "DBG " << component->name;
		if (component->name == label)
			return component;
	}
	return nullptr;
}

