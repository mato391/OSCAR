#include "stdafx.h"
#include "Router.hpp"
#include "ComponentFactory.hpp"

Router::Router(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger, Cache* cachePtr) : logger_(logger)
{
	BOOST_LOG(logger) << "INF " << "Router ctor";
	cache_ = cache;
	cachePtr_ = cachePtr;
	modLabel_ = "";
	mmfPath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\MMF.txt";
	startAutodetection();
	timeout_ = false;
	canPtr_ = new CAN(100);
	protoManager_ = new ProtocolManager(eqmObj_);
}

Router::~Router()
{
}

void Router::startAutodetection()
{
	BOOST_LOG(logger_) << "INF " << "Router::startAutodetection";
	createEQM();
	checkIfMMFExists();
	if (!fabricStartup_)
	{
		hwfService_ = new HWFService(eqmObj_, logger_, cachePtr_);
		hwfService_->prepareTopology();
	}
	startComponentService();
	BOOST_LOG(logger_) << "INF " << "Router::startAutodetection hwfTopologyDone. Waiting for hardware";
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
		BOOST_LOG(logger_) << "DEBUG " << "Router::StartComponentService: mmf size: " << mmfS_.size();
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
	BOOST_LOG(logger_) << "DBG " << "Router::createEQM";
	eqmObj_ = new EQM();
	cache_->push_back(eqmObj_);
	cachePtr_->addObject(eqmObj_);
}

void Router::startComponent(std::string name, std::string address)
{
	BOOST_LOG(logger_) << "INF " << "Router::startComponent: name " << name << " address: " << address;
	bool exist = false;
	for (auto &component : components_)
	{
		BOOST_LOG(logger_) << "DBG " << " Router::startComponent: found: " << component->name;
		if (name.find(component->name) != std::string::npos)
		{
			exist = true;
			break;
		}
	}
	if (!exist)
	{
		BOOST_LOG(logger_) << "INF " << "Router::startComponent: new component should be started " << name << " address: " << address;
		//componentsThreadGroup_.create_thread(std::bind(&ComponentFactory::createComponent, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		components_.push_back(ComponentFactory::createComponent(name, address, logger_));
		components_.back()->setCache(cache_);
		if (name.find("BDM") != std::string::npos)
			components_.back()->setCache(cachePtr_);
		components_.back()->setComponentsCache(&components_);
		components_.back()->setSenderPtr(std::bind(&Router::sender, this, std::placeholders::_1));
		components_.back()->initialize();
		
		//tutaj powinien pójsc runtime() componentsThreadGroup_.create_thread(std::bind(&Component::runtime, components_.back()));
	}
}

void Router::receiver(std::string data)
{
	if (!fabricStartup_)
	{
		if (canPtr_->messageAvailable())
		{
			canPtr_->receiveMessage();
			auto msg = protoManager_->createMessage(canPtr_->messageRx);

			//std::cout << "Router::receiver: message from domain: " << domain << std::endl;
			BOOST_LOG(logger_) << "INF " << "Router::receiver: msg available from: " << msg->fromDomain
				<< ", for: " << msg->toDomain << ", hdr " << msg->header << ", proto: " << static_cast<int>(msg->protocol);
			//BOOST_LOG(logger_) << "INFO " << "Router::receiver: " << data;
			if (msg->header == BB)
			{
				auto initMsg = static_cast<CMESSAGE::CInitialMessage*>(msg);
				BOOST_LOG(logger_) << "INF " << "Router::receiver: setup message has been received";
				setupModule(initMsg->fromDomain, initMsg->optional1 + initMsg->optional2);
				return;
			}
			for (const auto &mod : eqmObj_->modules_)
			{
				auto module = static_cast<MODULE*>(mod);
				if (module->domain == msg->fromDomain)
				{
					modLabel_ = module->label;
					break;
				}
			}
			for (const auto &component : components_)
			{
				BOOST_LOG(logger_) << "DBG " << "Router::receiver: Component: " << component->name;
				//BOOST_LOG(logger_) << "DEBUG " << "Router::receiver: Component configuringState: " << static_cast<int>(component->configuringState);
				//BOOST_LOG(logger_) << "DEBUG " << "Router::receiver: Component: domain " << component->domain;
				BOOST_LOG(logger_) << "DBG " << "Router::receiver: Component to find: " << modLabel_;
				if (modLabel_ != "" && modLabel_.find(component->name) != std::string::npos)
				{
					auto result = component->execute(msg);
					if (result != nullptr && result->getProtocol() != CMESSAGE::CMessage::EProtocol::CEmpty)
					{
						BOOST_LOG(logger_) << "INF " << "Router::receiver: transferring message to " << component->name;
						canPtr_->messageTx = protoManager_->createMessage(result);
						canPtr_->sendMessage();
						return;
					}
					else if (result != nullptr && result->getProtocol() == CMESSAGE::CMessage::EProtocol::CEmpty)
					{
						BOOST_LOG(logger_) << "ERR " << "Router::receiver: Resutl has been found - CEmpty: not require response";
					}
					else
						BOOST_LOG(logger_) << "ERR " << "Router::receiver: No result found";
				}
			}
			BOOST_LOG(logger_) << "ERR " << "Router::receiver: No component found" << modLabel_;
		}
	}
}


/*
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
*/
void Router::sender(CMESSAGE::CMessage* msg)
{
	BOOST_LOG(logger_) << "INF " << "Router::sender: msg sending to: " << msg->toDomain
		<< ", from: " << msg->fromDomain << ", hdr " << msg->header << ", proto: " << static_cast<int>(msg->protocol);
	auto can_msg = protoManager_->createMessage(msg);
	canPtr_->messageTx = can_msg;
	canPtr_->sendMessage();
}

void Router::checkIfMMFExists()
{
	BOOST_LOG(logger_) << "INF " << "Router::checkIfMMFExists";
	if (boost::filesystem::exists(mmfPath_))
	{
		fabricStartup_ = false;
		eqmObj_->configuringState = EQM::EConfiguringState::naked;
		BOOST_LOG(logger_) << "INF " << "Router::checkIfMMFExists: fabricStartup_: false";
	}
	else
	{
		eqmObj_->configuringState = EQM::EConfiguringState::configured;
		fabricStartup_ = true;
		BOOST_LOG(logger_) << "INF " << "Router::checkIfMMFExists: fabricStartup_: true";
	}
}

/*	HWPLANNE SERVICE CURRENTLY NOT SUPPORTED
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
*/

void Router::setupModule(std::string domain, int mask)
{
	BOOST_LOG(logger_) << "INF " << "Router::setupModule: " << domain << " mask: " << mask;
	for (const auto &mod : eqmObj_->modules_)
	{
		auto module = static_cast<MODULE*>(mod);
		if (module->domain == domain)
		{
			//BOOST_LOG(logger_) << "DBG " << domain << " is wanted. " << module->domain << " found";
			module->mask = std::to_string(mask);
			//BOOST_LOG(logger_) << "DBG " << module->mask;
			mIC_ = new ModuleInitialConfigurator(module, logger_);
			for (const auto &mod : eqmObj_->modules_)
			{
				auto module = static_cast<MODULE*>(mod);
				if (module->domain == domain)
					modLabel_ = module->label;
			}
			//BOOST_LOG(logger_) << "DBG " << "Router::receiver modLabel: " << modLabel_;
			for (const auto &component : components_)
			{
				BOOST_LOG(logger_) << "DBG " << "Router::setupModule component: " << component->name;
				if (modLabel_ != "" && modLabel_.find(component->name) != std::string::npos)
				{
					int idomain = std::stoi(module->domain.substr(2, 2));
					auto result = component->setup(idomain);
					BOOST_LOG(logger_) << "INF " << "Router::setupModule: Component setup "
						<< component->name << " done, with status: "
						<< static_cast<int>(result->status) << " | domain: " << idomain;
					if (result != nullptr && result->status == RESULT::EStatus::success)
					{
						BOOST_LOG(logger_) << "INF " << "Router::setupModule: " << "Sending Protocol setup message";
						canPtr_->messageTx = protoManager_->createProtocolNegotatorMessage(std::stoi(result->feedback), std::to_string(idomain));
						canPtr_->sendMessage();
						BOOST_LOG(logger_) << "INF " << "Router::setupModule: Message sent";
						//assert(1 != 1);
						delete result;	//POSSIBLE CRASH
					}
				}
			}
			BOOST_LOG(logger_) << "INF " << "Router::setupModule: done";
			return;
		}
	}
}

