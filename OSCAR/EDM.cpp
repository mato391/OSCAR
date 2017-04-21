#include "stdafx.h"
#include "EDM.hpp"


EDM::EDM(std::string domain, boost::log::sources::logger_mt logger)
{
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
		engineObj_->engineType = static_cast<ENGINE::EType>(std::stoi(message.substr(5, 1)));
		engineObj_->petrolType = static_cast<ENGINE::EPetrolType>(std::stoi(message.substr(6, 1)));
		engineObj_->detectionState = ENGINE::EDetectionState::online;
		return;
	}
	if (message.find( "0x0302" ) != std::string::npos)
	{
		checkIfEngineStarted(message.substr(6, message.size() - 6));
		return;
	}
	if (message.find("0x0303") != std::string::npos)
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
	if (imo->operation == "START_STOP_ENGINE" && imo->details == "1")
	{
		startStopEngineProcedure_ = new StartStopEngineProcedure(engineObj_);
		auto result = startStopEngineProcedure_->getResult();
		if ( result.status == RESULT::EStatus::success)
		{
			BOOST_LOG(logger_) << "INFO " << "EDM::execute: starting engine procedure";
			//send(result.feedback);
		}
		
	}
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
	std::fstream mapFile("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\engineMap_" + std::to_string(pos) + ".txt", std::ios::in);
	std::string map;
	mapFile >> map;
	mapFile.close();
	send("0x0304");
}

