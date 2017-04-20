#include "stdafx.h"
#include "UCM.hpp"


UCM::UCM(std::string domain, boost::log::sources::logger_mt logger)
{
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
		}
	}
}

void UCM::execute(INTER_MODULE_OPERATION* imo)
{

}

void UCM::startUserOperationProcessing(BUTTON* button)
{
	if (button->label == "START_STOP_BUTTON")
	{
		BOOST_LOG(logger_) << "INFO " << "UCM::startUserOperationProcessing: " << button->label << " pressed. Starting EDM operation";
		int preconditions = checkPreconditionToStartEngine();
		for (const auto &component : *componentCache_)
		{
			if (component->name == "EDM")
			{
				if (preconditions == 2)
				{
					BOOST_LOG(logger_) << "INFO " << "UCM::startUserOperationProcessing: power on execution";
					BOOST_LOG(logger_) << "INFO " << "UCM::startUserOperationProcessing: start engine execution";

					component->execute(new INTER_MODULE_OPERATION("ENGINE_START_TASK", "1"));
					return;
				}
				else if (preconditions == 1)
				{
					BOOST_LOG(logger_) << "INFO " << "UCM::startUserOperationProcessing: power on execution";
				}
			}
			BOOST_LOG(logger_) << "ERROR " << "UCM::startUserOperationProcessing: "  << " Component cannot be found";
			return;
		}
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