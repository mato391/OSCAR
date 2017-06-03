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
	startInternalModuleAutodetection();
	if (!fabricStartup_)
	{
		hwfService_ = new HWFService(eqmObj_);
		hwfService_->prepareTopology();
	}
	
	//check and start WCM module
	//start script to run ipconfig/ifconfig -> file with response
	//if file exists initialize WCM with subcomponen WirelessCardModule
	BOOST_LOG(logger_) << "INF " << "startAutodetection hwfTopologyDone. Waiting for hardware";
}

void Router::startInternalModuleAutodetection()
{

	components_.push_back(ComponentFactory::createComponent("WCM", "0x07", logger_));
	components_.back()->setCache(cache_);
	components_.back()->execute(new INTER_MODULE_OPERATION("ETHERNET_INIT", ""));
	components_.back()->setComponentsCache(&components_);
	components_.back()->setSenderPtr(std::bind(&Router::sender, this, std::placeholders::_1));
	components_.back()->configuringState = Component::EConfiguringState::initialized;
}

void Router::startComponentService()
{
	//createEQM();
	checkIfMMFExists();
	if (!fabricStartup_)
	{
		
		std::fstream mmf(mmfPath_, std::ios::in);
		std::string mmf_;
		mmf >> mmf_;
		mmf.close();
		
		BOOST_LOG(logger_) << "INFO " << "ROUTER:StartComponentService: mmf: " << mmf_;
		boost::split(mmfS_, mmf_, boost::is_any_of(";"));
		BOOST_LOG(logger_) << "DEBUG " << "ROUTER:StartComponentService: mmf size: " << mmfS_[0];
		for (const auto &line : mmfS_)
		{
			if (line != "")
			{
				std::vector<std::string> componentAddress;
				//std::cout << "ROUTER:StartComponentService: mmf: " << line << std::endl;
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
	BOOST_LOG(logger_) << "DBG " << "startComponent " << name << " " << address;
	bool exist = false;
	for (auto &component : components_)
	{
		BOOST_LOG(logger_) << "DBG " << " startComponent found: " << component->name;
		if (name.find(component->name) != std::string::npos)
		{
			exist = true;
			break;
		}
	}
	if (!exist)
	{
		components_.push_back(ComponentFactory::createComponent(name, address, logger_));
	}
}

void Router::initializeComponent(std::string name)
{
	BOOST_LOG(logger_) << "DBG " << "INITIALIZE COMPONENT " << name;
	for (auto &component : components_)
	{
		if (name.find(component->name) != std::string::npos)
		{
			if (component->configuringState != Component::EConfiguringState::initialized)
			{
				component->setCache(cache_);
				component->setComponentsCache(&components_);
				component->setSenderPtr(std::bind(&Router::sender, this, std::placeholders::_1));
				component->configuringState = Component::EConfiguringState::initialized;
			}
			BOOST_LOG(logger_) << "DBG " << "INITIALIZE COMPONENT start init for subcomponent: " << name;
			component->initialize(name);
		}
	}
}

void Router::receiver(std::string data)
{
	
	if (!fabricStartup_)
	{
		std::string domain = data.substr(0, 4);
		BOOST_LOG(logger_) << "INFO " << "Router::receiver: " << domain;
		for (const auto &component : components_)
		{
			BOOST_LOG(logger_) << "DEBUG " << "Router::receiver: Component: " << component->name;
			BOOST_LOG(logger_) << "DEBUG " << "Router::receiver: Component configuringState: " << static_cast<int>( component->configuringState);
			BOOST_LOG(logger_) << "DEBUG " << "Router::receiver: Component: domain " << component->domain;
			if (component->configuringState == Component::EConfiguringState::configured && domain.find(component->domain) != std::string::npos)
			{
				BOOST_LOG(logger_) << "INFO " << "Router::receiver: transferring message to " << component->name;
				component->execute(data);
				break;
			}
			
			BOOST_LOG(logger_) << "INFO " << "Router::receiver: No component found" ;
		}
	}
	moduleAutodetection(data);
}

void Router::checkResultFromHWPlannerService()
{
	BOOST_LOG(logger_) << "DBG " << "Router::checkResultFromHWPlannerService";
	for (auto &obj : *cache_)//crash
	{
		BOOST_LOG(logger_) << "DBG " << "Router::checkResultFromHWPlannerService: " << obj->name;
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

void Router::moduleAutodetection(std::string data)
{
	if (timer_ != nullptr)
		delete timer_;
	bool exist = false;
	if (data.substr(4, 4) == "0000")
	{
		BOOST_LOG(logger_) << "INF " << "Router::moduleAutodetection: welcomeMessage received from module " << data.substr(0, 4);
		for (const auto &mod : eqmObj_->modules_)
		{
			if (data.substr(0, 4) == static_cast<MODULE*>(mod)->domain)
			{
				exist = true;
				break;
			}

		}
		BOOST_LOG(logger_) << "DBG " << "Router::moduleAutodetection: exist: " << exist;
		if (!exist)
		{
			MODULE* module = new MODULE();
			module->domain = data.substr(0, 4);
			module->detectionStatus = MODULE::EDetectionStatus::online;
			eqmObj_->addModule(module);
			sender(module->domain + "FF");
			BOOST_LOG(logger_) << "INF " << "Router::moduleAutodetection: module created " << module->domain << " " << module->label;
		}
		else
		{
			for (auto &mod : eqmObj_->modules_)
			{
				auto module = static_cast<MODULE*>(mod);
				if (module->domain == data.substr(0, 4))
				{
					module->detectionStatus = MODULE::EDetectionStatus::online;
					sender(module->domain + "FF");
					BOOST_LOG(logger_) << "INF " << "Router::moduleAutodetection: module created " << module->domain << " " << module->label;
				}
			}
		}
	}
	else if (data.substr(4, 2) == "EE")
	{
		for (const auto &mod : eqmObj_->modules_)
		{
			auto module = static_cast<MODULE*>(mod);
			BOOST_LOG(logger_) << "DBG " << "moduleAutodetection: " << module->label;
			if (data.substr(0, 4) == module->domain)
			{
				module->mask = data.substr(6, 6);
				if (fabricStartup_)
					setupModule(module);
				module->operationalState = MODULE::EOperationalState::configured;
				initializeComponent(module->label);
				break;
			}

		}
	}
	else if (data.substr(4, 1) == "9" )
	{
		BOOST_LOG(logger_) << "INF " << "Router::moduleAutodetection: info data ";
		for (const auto &mod : eqmObj_->modules_)
		{
			auto module = static_cast<MODULE*>(mod);
			BOOST_LOG(logger_) << "DBG " << "WHY IT IS: " << module->label;
			if (data.substr(0, 4) == module->domain )
			{
				module->serialNumber = data.substr(4, 4);
				module->productNumber = data.substr(8, 6);
				if (fabricStartup_)
					createConnectors(module);
				module->operationalState = MODULE::EOperationalState::enabled;
				BOOST_LOG(logger_) << "DEBUG starting Component for module: " << module->name;
				startComponent(module->label, module->domain);
				break;
			}
		}
	}
	if (task == nullptr)
	{
		task = new TASK("HWPlannerService", std::bind(&Router::checkResultFromHWPlannerService, this));
		cache_->push_back(task);
		startHWPlanerService();
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
		
	//ShellExecute(NULL, "open", "", NULL, NULL, SW_SHOWDEFAULT);
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