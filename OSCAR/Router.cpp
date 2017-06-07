#include "stdafx.h"
#include "Router.hpp"
#include "ComponentFactory.hpp"

Router::Router(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger) : logger_(logger)
{
	cache_ = cache;
	mmfPath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\MMF.txt";
	startAutodetection();
	timeout_ = false;
}


Router::~Router()
{
}
void Router::startAutodetection()
{
	createEQM();
	checkIfMMFExists();
	if (!fabricStartup_)
	{
		hwfService_ = new HWFService(eqmObj_);
		hwfService_->prepareTopology();
	}
	startComponentService();
	BOOST_LOG(logger_) << "INF " << "startAutodetection hwfTopologyDone. Waiting for hardware";
}


void Router::startComponentService()
{
	checkIfMMFExists();
	if (!fabricStartup_)
	{
		std::fstream mmf(mmfPath_, std::ios::in);
		std::string mmf_;
		mmf >> mmf_;
		mmf.close();
		
		BOOST_LOG(logger_) << "INFO " << "Router::StartComponentService: mmf: " << mmf_;
		boost::split(mmfS_, mmf_, boost::is_any_of(";"));
		BOOST_LOG(logger_) << "DEBUG " << "Router::StartComponentService: mmf size: " << mmfS_[0];
		for (const auto &line : mmfS_)
		{
			if (line != "")
			{
				std::vector<std::string> componentAddress;
				boost::split(componentAddress, line, boost::is_any_of(":"));
				startComponent(componentAddress[0], componentAddress[1]);
			}
		}
	}
	else
	{
		BOOST_LOG(logger_) << "INFO " << "ROUTER:StartComponentService: fabric startup with naked sw";
	}
	
	
}

void Router::createEQM()
{
	eqmObj_ = new EQM();
	cache_->push_back(eqmObj_);
}

void Router::startComponent(std::string name, std::string address)
{
	BOOST_LOG(logger_) << "INF " << "startComponent " << name << " " << address;
	bool exist = false;
	for (auto &component : components_)
	{
		//BOOST_LOG(logger_) << "DBG " << " startComponent found: " << component->name;
		if (name.find(component->name) != std::string::npos)
		{
			exist = true;
			break;
		}
	}
	if (!exist)
	{
		components_.push_back(ComponentFactory::createComponent(name, address, logger_));
		components_.back()->setCache(cache_);
		components_.back()->setComponentsCache(&components_);
		components_.back()->setSenderPtr(std::bind(&Router::sender, this, std::placeholders::_1));
		//components_.back()->initialize();
	}
}


void Router::receiver(std::string data)
{
	if (!fabricStartup_)
	{
		std::string domain = data.substr(0, 4);
		std::string modLabel = "";
		BOOST_LOG(logger_) << "INFO " << "Router::receiver: " << data;
		for (const auto &mod : eqmObj_->modules_)
		{
			auto module = static_cast<MODULE*>(mod);
			if (module->domain == domain)
				modLabel = module->label;
		}
		for (const auto &component : components_)
		{
			//BOOST_LOG(logger_) << "DEBUG " << "Router::receiver: Component: " << component->name;
			//BOOST_LOG(logger_) << "DEBUG " << "Router::receiver: Component configuringState: " << static_cast<int>(component->configuringState);
			//BOOST_LOG(logger_) << "DEBUG " << "Router::receiver: Component: domain " << component->domain;
			if (modLabel != "" && modLabel.find(component->name) != std::string::npos)
			{
				BOOST_LOG(logger_) << "INFO " << "Router::receiver: transferring message to " << component->name;
				component->execute(data);
				break;
			}

			BOOST_LOG(logger_) << "INFO " << "Router::receiver: No component found";
		}
	}
}

void Router::checkResultFromHWPlannerService()
{
	//BOOST_LOG(logger_) << "DBG " << "Router::checkResultFromHWPlannerService";
	for (auto &obj : *cache_)//crash
	{
		//BOOST_LOG(logger_) << "DBG " << "Router::checkResultFromHWPlannerService: " << obj->name;
		if (obj->name == "RESULT" && static_cast<RESULT*>(obj)->applicant == "HWPlannerService" && static_cast<RESULT*>(obj)->feedback == "CONFIGURATION_DONE")
		{
			BOOST_LOG(logger_) << "INF " << "Router::checkResultFromHWPlannerService: Configuration done.Starting with hw";
			fabricStartup_ = false;
			createMMFFromEQM();
			startComponentService();
			break;
		}
	}
}

void Router::sender(std::string data)
{
	while (boost::filesystem::exists("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_send.txt"))
	{
		boost::this_thread::sleep(boost::posix_time::millisec(50));
	}
	std::fstream file("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_send.txt", std::ios::out);
	if (file.good())
	{
		std::string content;
		file << data;
		file.close();
		//std::remove("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt");
		BOOST_LOG(logger_) << "INFO " << "Router::sender: CAN sending signal: " << data;
		//router->receiver(content);
	}
}

void Router::checkIfMMFExists()
{
	if (boost::filesystem::exists(mmfPath_))
	{
		fabricStartup_ = false;
		eqmObj_->configuringState = EQM::EConfiguringState::naked;
		return;
	}
	else
	{
		eqmObj_->configuringState = EQM::EConfiguringState::configured;
		fabricStartup_ = true;
	}
}

void Router::setupTimer()
{
	BOOST_LOG(logger_) << "INF " << "Router::setupTimer";
	bool timeout = false;
	timer_ = new TIMER(25, "detectionTimeout");
	boost::thread t(std::bind(&TIMER::start, timer_, std::placeholders::_1), &timeout_);
}

void Router::startHWPlanerService()
{
	if (hwplannerService_ == nullptr)
	{
		BOOST_LOG(logger_) << "INF " << "Router::startHWPlanerService: start";
		hwplannerService_ = new HWPlannerService(logger_, cache_);
		hwPlannerServiceThread_ = boost::thread(std::bind(&HWPlannerService::startReceiving, hwplannerService_));
	}
	else
		BOOST_LOG(logger_) << "INF " << "Router::startHWPlanerService: hwPlanner has been started";
}

void Router::createConnectors(MODULE* mod)
{
	BOOST_LOG(logger_) << "INF " << "Router::createConnectors: for " << mod->productNumber;
	int group = mod->productNumber.size() / 2;
	BOOST_LOG(logger_) << "INF " << "Router::createConnectors: groups:  " << group;
	std::vector<int> connectorsInGroups;
	for (int i = 0; i < group; i++)
	{
		BOOST_LOG(logger_) << "INF " << "Router::createConnectors: group:  " << std::stoi(mod->productNumber.substr(i * 2, 2));
		connectorsInGroups.push_back(std::stoi(mod->productNumber.substr(i * 2, 2)));
	}
	for (const auto &conn : connectorsInGroups)
	{
		mod->addConnector(conn);
	}
}

void Router::displayModulesTopology()
{
	for (const auto &mod : eqmObj_->modules_)
	{
		auto module = static_cast<MODULE*>(mod);
		BOOST_LOG(logger_) << "DBG " << "MODULE: " << module->domain;
		int i = 0;
		for (const auto &group : module->connectors_)
		{
			BOOST_LOG(logger_) << "DBG " << "CONNECTORS_GROUP: " << i;
			for (const auto &conn : group)
			{
				BOOST_LOG(logger_) << "DBG " << "CONNECTOR: " << static_cast<CONNECTOR*>(conn)->id
					<< " used " << static_cast<CONNECTOR*>(conn)->used;
			}
			i++;
		}
	}
}

void Router::setupModule(Obj* mod)
{
	auto module = static_cast<MODULE*>(mod);
	int group = module->mask.size() / 2;
	std::vector<int> masks;
	for (int i = 0; i < group; i++)
	{
		masks.push_back(std::stoi(module->mask.substr(i * 2, 2)));
	}
	for (int i = 0; i < masks.size(); i++)
	{
		BOOST_LOG(logger_) << "DBG " << "Router::setupModule: mask: " << i << " " << masks[i];
		for (int j = 0; j < masks[i]; j++)
		{
			static_cast<CONNECTOR*>(module->connectors_[i][j])->used = true;
		}
	}
	displayModulesTopology();
}

void Router::createMMFFromEQM()
{
	std::fstream mmfh("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\MMF.txt", std::ios::app);
	for (const auto &mod : eqmObj_->modules_)
	{
		auto module = static_cast<MODULE*>(mod);
		std::string msg = module->label + ":" + module->domain + ";";
		mmfh << msg;
	}
	mmfh.close();
}