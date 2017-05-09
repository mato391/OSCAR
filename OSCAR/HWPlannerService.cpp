#include "stdafx.h"
#include "HWPlannerService.hpp"


HWPlannerService::HWPlannerService(boost::log::sources::logger_mt logger, std::vector<Obj*>* cache) : logger_(logger)
{
	cache_ = cache;
	for (auto &obj : *cache_)
	{
		if (obj->name == "EQM")
		{
			eqmObjPtr_ = static_cast<EQM*>(obj);
			break;
		}
	}
}


HWPlannerService::~HWPlannerService()
{
}

void HWPlannerService::startReceiving()
{
	for (;;)
	{

		std::fstream file(commStreamRecvPath_, std::ios::in);
		if (file.good())
		{
			std::string content;
			file >> content;
			file.close();
			std::remove("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\hwps_recv.txt");
			BOOST_LOG(logger_) << "INFO " << boost::this_thread::get_id() <<  " HWPlannerService::startReceiving: handling message: " << content;
			handleMessage(content);

		}

		boost::this_thread::sleep(boost::posix_time::millisec(50));

	}
}

void HWPlannerService::handleMessage(std::string message)
{
	if (message == "CONFIGURATION_DONE")
	{
		loadHWF();
		createMMF();
	}
}

void HWPlannerService::loadHWF()
{
	std::fstream file(hwfFilePath_, std::ios::in);
	if (file.good())
	{
		file >> hwfContent_;
		file.close();
	}
}

void HWPlannerService::createMMF()
{
	std::vector<std::string> shwfContent;
	boost::split(shwfContent, hwfContent_, boost::is_any_of(";"));
	for (const auto &module : shwfContent)
	{
		createModule(module);
	}
	//delete &shwfContent;	//CRASH here to check
}

void HWPlannerService::createModule(std::string mod)
{
	BOOST_LOG(logger_) << "INF " << "HWPlannerService::createModule " << mod;
	std::vector<std::string> smod;
	boost::split(smod, mod, boost::is_any_of("|"));
	std::string moduleSn;
	int connectorsGroup;
	for (const auto &conn : smod)
	{
		BOOST_LOG(logger_) << "DBG " << conn << " size of smod: " << smod.size();
		if (conn.find("Module") != std::string::npos)
		{
			std::vector<std::string> sconn;
			boost::split(sconn, conn, boost::is_any_of(":"));
			std::vector<std::string> sconnp;
			boost::split(sconnp, sconn[1], boost::is_any_of(","));
			moduleSn = sconnp[0];
			BOOST_LOG(logger_) << "INF " << "HWPlannerService::createModule: module serialNumber: " << sconnp[0];
		}
		if (conn.find("ConnectorsGroup") != std::string::npos)
		{
			std::vector<std::string> sconn;
			boost::split(sconn, conn, boost::is_any_of(":"));
			std::vector<std::string> sconnp;
			boost::split(sconnp, sconn[1], boost::is_any_of(","));
			connectorsGroup = std::stoi(sconnp[0]);
			BOOST_LOG(logger_) << "INF " << "HWPlannerService::createModule: connectorsGroup: " << sconnp[0];
		}
		if (conn.find("Connector:") != std::string::npos)
		{
			std::vector<std::string> sconn;
			boost::split(sconn, conn, boost::is_any_of(":"));
			std::vector<std::string> sconnp;
			boost::split(sconnp, sconn[1], boost::is_any_of(","));
			BOOST_LOG(logger_) << "DBG " << "Connector: " << sconn[1] << " size " << sconnp.size();
			setConnector(std::stoi(sconnp[0]), sconnp[1], connectorsGroup, moduleSn); 
		}
	}
}

void HWPlannerService::setConnector(int id, std::string label, int connectorGroup, std::string moduleSn)
{
	MODULE* module = nullptr;
	for (auto &mod : eqmObjPtr_->modules_)
	{
		if (static_cast<MODULE*>(mod)->serialNumber == moduleSn)	//CRASH
		{
			module = static_cast<MODULE*>(mod);
			break;
		}
	}
	if (module != nullptr)
	{
		BOOST_LOG(logger_) << "INF " << "HWPlannerService::setConnector: setConnector in module: " << module->domain;
		BOOST_LOG(logger_) << "INF " << "HWPlannerService::setConnector: connectors in module: " << module->connectors_.size();
		for (auto &conn : module->connectors_[connectorGroup])
		{
			BOOST_LOG(logger_) << "DBG " << static_cast<CONNECTOR*>(conn)->id;
			if (static_cast<CONNECTOR*>(conn)->id == id)
			{
				static_cast<CONNECTOR*>(conn)->label = label;
				BOOST_LOG(logger_) << "INF " << "HWPlannerService::setConnector: setConnector in connectorsGroup: "
					<< connectorGroup << " , connector id: " << id;
			}
		}
		//delete module;
	}
	
}
