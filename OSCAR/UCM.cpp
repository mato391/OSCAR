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
	prepareSwitchTopology();
	startStopEngineButtonAgent_ = new StartStopEngineButtonAgent(switchTopology, logger_);
	
}

void UCM::prepareSwitchTopology()
{
	switchTopology = new SWITCH_TOPOLOGY();
	std::fstream scfFile("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\SCF.txt", std::ios::in);
	std::string scfContain;
	scfFile >> scfContain;
	scfFile.close();
	boost::split(scfVec_, scfContain, boost::is_any_of(";"));
	scfObj_ = new SCF();
	scfObj_->scfPath = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\SCF.txt";
	prepareTopologyBasedOnSCF();
}

void UCM::prepareTopologyBasedOnSCF()
{
	BOOST_LOG(logger_) << "INFO " << "UCM::prepareTopologyBasedOnSCF";
	for (const auto &line : scfVec_)
	{
		std::vector<std::string> sline;
		boost::split(sline, line, boost::is_any_of(":"));
		if (sline[0].find("BUTTON") != std::string::npos)
		{
			BUTTON* button = new BUTTON();
			button->label = sline[0];
			button->port = sline[1];
			switchTopology->buttonTopology.push_back(button);
			
			
		}
		else if (sline[0].find("PEDAL") != std::string::npos)
		{
			PEDAL* pedal = new PEDAL();
			pedal->label = sline[0];
			pedal->port = sline[1];
			switchTopology->pedals.push_back(pedal);
		}
	}	
	displayTopology();
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
	//0x02yyz; yy - port, z - operation (position)
	std::string port = message.substr(4, 2);
	std::string operation = message.substr(6, 1);
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
			pedal->isPushed = static_cast<bool>(std::stoi(operation));	//SHOULD BE CHANGED
			startPedalOperation(pedal);
			return;
		}
	}

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
	BOOST_LOG(logger_) << "DBG " << "UCM::getComponent " << label;
	for (const auto &component : *componentCache_)
	{
		BOOST_LOG(logger_) << "DBG " << component->name;
		if (component->name == label)
			return component;
	}
	return nullptr;
}

