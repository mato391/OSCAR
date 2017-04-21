#include "stdafx.h"
#include "EDM.hpp"


EDM::EDM(std::string domain, boost::log::sources::logger_mt logger)
{

	this->name = "EDM";
	this->domain = domain;
	this->logger_ = logger;
	BOOST_LOG(logger_) << "DEBUG " << "EDM ctor";
	initialize();
}

EDM::~EDM()
{
}

void EDM::initialize()
{
	engineObj_ = new ENGINE();
	engineObj_->detectionState = ENGINE::EDetectionState::offline;
	engineObj_->milage = getMilageFromPersist();
	engineObj_->proceduralState = ENGINE::EProceduralState::turnedOff;
	engineObj_->startFailures = 0;
	startDetection();
	
}

void EDM::execute(std::string message)
{
	if (message == "0x0300")
	{
		send("0x0301");
		engineObj_->detectionState = ENGINE::EDetectionState::connecting;
		return;
	}
	if (message.find( "0x0301" ) != std::string::npos)
	{
		setupEngine(static_cast<ENGINE::EType>(std::stoi(message.substr(6, 1))), static_cast<ENGINE::EPetrolType>(std::stoi(message.substr(7, 1))));
		loadEngineMaps(1);
		return;
	}
	if (message.find( "0x0302" ) != std::string::npos)
	{
		checkIfEngineStarted(message.substr(6, message.size() - 6));
		return;
	}
	if (message.find("0x0304") != std::string::npos)
	{
		if (message.back() == '0')
		{
			engineObj_->proceduralState = ENGINE::EProceduralState::configured;
			BOOST_LOG(logger_) << "INF " << "Maps has been applied. ENGINE is " << static_cast<int>(engineObj_->proceduralState);
		}
		else
		{
			BOOST_LOG(logger_) << "ERR " << "Engine doesn't have maps";
		}
	}
	if (message.find("0x0305") != std::string::npos)
	{
		if (engineObj_->rpm != std::stoi(message.substr(6, message.size() - 6)))
		{
			engineObj_->rpm = std::stoi(message.substr(6, message.size() - 6));
		}
		return;
	}


}

void EDM::execute(INTER_MODULE_OPERATION* imo)
{
	if (startStopEngineProcedure_ == nullptr)
		startStopEngineProcedure_ = new StartStopEngineProcedure(engineObj_);
	if (imo->operation == "START_STOP_ENGINE" && imo->details == "1")
	{
		auto result = startStopEngineProcedure_->getResult();
		if ( result.status == RESULT::EStatus::success)
		{
			BOOST_LOG(logger_) << "INFO " << "EDM::execute: starting engine procedure";
			rpmMonitor_ = new RpmMonitor(engineObj_, logger_);
			boost::thread rpmMonitorStart(std::bind(&RpmMonitor::start, rpmMonitor_));
			rpmMonitorStart.detach();
			send("0x0302");
		}
	}
	else if (imo->operation == "CLUTCH_ENGINE")
	{
		engineObj_->operationalState = static_cast<ENGINE::EOperationalState>(std::stoi(imo->details));
	}
}

void EDM::setupEngine(ENGINE::EType type, ENGINE::EPetrolType petrolType)
{
	engineObj_->engineType = type;
	engineObj_->petrolType = petrolType;
	engineObj_->detectionState = ENGINE::EDetectionState::online;
	BOOST_LOG(logger_) << "EDM::setupEngine " << "ENGINE has been detected " << static_cast<int>(engineObj_->engineType) << " " << static_cast<int>(engineObj_->petrolType);
}

void EDM::checkIfEngineStarted(std::string data)
{
	RESULT* result = new RESULT();
	
	if (data == "0000")
	{
		engineObj_->proceduralState = ENGINE::EProceduralState::startedIdle;
		//boost::thread t(std::bind(&EDM::rpmMonitor, this));
		//t.join();
		result->applicant = "EDM";
		result->status = RESULT::EStatus::success;
		engineObj_->children.push_back(result);
		return;
	}
	engineObj_->alarms.push_back(new ALARM());
	engineObj_->alarms.back()->alarmCode = std::stoi(data);


	if (data == "FFFF")
	{
		engineObj_->startFailures++;
	}

}

void EDM::rpmMonitor()
{
	BOOST_LOG(logger_) << "INFO " << "EDM::rpmMonitor started";
}

bool EDM::checkPreconditionsToStartEngine()
{
	if (engineObj_->operationalState == ENGINE::EOperationalState::clutched)
		return true;
	return false;
}

int EDM::getMilageFromPersist()
{
	std::fstream milageFile("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\Persist\\Milage.txt", std::ios::in);
	std::string sMilage;
	milageFile >> sMilage;
	milageFile.close();
	return std::stoi(sMilage);
}

void EDM::startDetection()
{
	//send("0x0300");	
}

void EDM::loadEngineMaps(int pos)
{
	BOOST_LOG(logger_) << "INF " << "EDM::loadEngineMaps: " << pos;
	send("0x0304" + std::to_string(pos));
}

