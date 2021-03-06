#include "stdafx.h"
#include "Router.hpp"
#include "ComponentFactory.hpp"
#define DEBUG true

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
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	createEQM();
	checkIfMMFExists();
	if (!fabricStartup_)
	{
		hwfService_ = new HWFService(eqmObj_, logger_, cachePtr_);
		hwfService_->prepareTopology();
		cM_ = new ConnectorManager(logger_, cachePtr_);
		cM_->getModules();
	}
	startComponentService();
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " hwfTopologyDone. Waiting for hardware";
	
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
		
		BOOST_LOG(logger_) << "INFO " << __FUNCTION__ << " mmf: " << mmf_;
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
	BOOST_LOG(logger_) << "INF " << __FUNCTION__<<" name " << name << " address: " << address;
	bool exist = false;
	for (auto &component : components_)
	{
		//BOOST_LOG(logger_) << "DBG " << " Router::startComponent: found: " << component->name;
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
		if (name.find("BDM") != std::string::npos || name.find("UIA") != std::string::npos || name.find("UCM") != std::string::npos)
			components_.back()->setCache(cachePtr_);
		components_.back()->setComponentsCache(&components_);
		components_.back()->setSenderPtr(std::bind(&Router::sender, this, std::placeholders::_1));
		components_.back()->initialize();
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
			BOOST_LOG(logger_) << "INF " << "Router::receiver: msg available from: " << msg->fromDomain
				<< ", for: " << msg->toDomain << ", hdr " << msg->header << ", proto: " << static_cast<int>(msg->protocol);
			if (ROUTER_DBG)
			{
				BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " Data[0] : " << static_cast<int>(canPtr_->messageRx.data[0])
					<< " Data[1] : " << static_cast<int>(canPtr_->messageRx.data[1])
					<< " Data[2] : " << static_cast<int>(canPtr_->messageRx.data[2])
					<< " Data[3] : " << static_cast<int>(canPtr_->messageRx.data[3])
					<< " Data[4] : " << static_cast<int>(canPtr_->messageRx.data[4])
					<< " Data[5] : " << static_cast<int>(canPtr_->messageRx.data[5])
					<< " Data[6] : " << static_cast<int>(canPtr_->messageRx.data[6]);
			}
			//BOOST_LOG(logger_) << "INFO " << "Router::receiver: " << data;
			if (cM_ != nullptr && (msg->protocol == CMESSAGE::CMessage::EProtocol::CMaskExtendedProtocol
				|| msg->protocol == CMESSAGE::CMessage::EProtocol::CMaskProtocol 
				|| (msg->protocol == CMESSAGE::CMessage::EProtocol::CInitialProtocol && msg->header == 187)))		//DONE do not change
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " Protocol 6 or 7 detected. Going to ConnectorManager";
				cM_->handleMaskConnectorChange(msg);
				if (msg->header == BB)
				{
					auto initMsg = static_cast<CMESSAGE::CInitialMessage*>(msg);
					BOOST_LOG(logger_) << "INF " << "Router::receiver: setup message has been received";
					setupModule(initMsg->fromDomain, initMsg->optional1 + initMsg->optional2);
				}
				return;
			}
			else if (cM_ != nullptr && msg->protocol == CMESSAGE::CMessage::EProtocol::CSimpleProtocol)
			{
				BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " Protocol2 detected. Going to ConnectorManager";
				cM_->handleSingleConnectorChange(msg);
				return;
			}
			if (msg->header == AA)		//DONE do not change
			{
				BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " AA message detected";
				for (const auto &mod : eqmObj_->children)
				{
					auto module = static_cast<MODULE*>(mod);
					BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << "module to check " << module->domain << " " << module->label;
					if (module->domain == msg->fromDomain)
					{
						BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << "module found " << module->domain << " " << module->label;
						modLabel_ = module->label;
						break;
					}
				}
				for (const auto &component : components_)
				{
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
							BOOST_LOG(logger_) << "ERR " << "Router::receiver: Result has been found - CEmpty: not required response";
						}
						else
							BOOST_LOG(logger_) << "ERR " << "Router::receiver: No result found";
					}
				}
			}
		}
	}
	else
	{
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " it should be determine";
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
	if (ROUTER_DBG)
	{
		BOOST_LOG(logger_) << "DBG " << "Router::sender: messageCAN: data[0]" << static_cast<int>(canPtr_->messageTx.data[0])
			<< " data[1] " << static_cast<int>(canPtr_->messageTx.data[1])
			<< " data[2] " << static_cast<int>(canPtr_->messageTx.data[2])
			<< " data[3] " << static_cast<int>(canPtr_->messageTx.data[3])
			<< " data[4] " << static_cast<int>(canPtr_->messageTx.data[4])
			<< " data[5] " << static_cast<int>(canPtr_->messageTx.data[5])
			<< " data[6] " << static_cast<int>(canPtr_->messageTx.data[6])
			<< " data[7] " << static_cast<int>(canPtr_->messageTx.data[7]);
	}
	
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
	auto moduleObjs = cachePtr_->getAllObjectsUnder(eqmObj_, "MODULE");
	MODULE* moduleForSetup_ = new MODULE();
	moduleForSetup_->domain = "XX";
	for (const auto &mod : moduleObjs)
	{
		auto module = static_cast<MODULE*>(mod);
		if (module->domain == domain)
		{
			BOOST_LOG(logger_) << "INF " << __FUNCTION__ << "moduleForSetup put";
			moduleForSetup_ = module;
			break;
		}
	}
	//BOOST_LOG(logger_) << "DBG " << "Router::receiver modLabel: " << modLabel_;
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " put module " << moduleForSetup_->label;
	for (const auto &component : components_)
	{
		BOOST_LOG(logger_) << "DBG " << "Router::setupModule component: " << component->name;
		if (moduleForSetup_->domain != "XX" && moduleForSetup_->label.find(component->name) != std::string::npos)
		{
			BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " Trying to setup component";
			int idomain = std::stoi(moduleForSetup_->domain.substr(2, 2));
			auto result = component->setup(idomain);
			BOOST_LOG(logger_) << "INF " << "Router::setupModule: Component setup "
				<< component->name << " done, with status: "
				<< static_cast<int>(result->status) << " | domain: " << idomain;
			if (result != nullptr && result->status == RESULT::EStatus::success)
			{
				BOOST_LOG(logger_) << "INF " << "Router::setupModule: " << "Sending Protocol setup message";
				canPtr_->messageTx = protoManager_->createProtocolNegotatorMessage(std::stoi(result->feedback), std::to_string(idomain));
				if (ROUTER_DBG)
				{
					BOOST_LOG(logger_) << "DBG " << "Router::sender: messageCAN: data[0]" << static_cast<int>(canPtr_->messageTx.data[0])
						<< " data[1] " << static_cast<int>(canPtr_->messageTx.data[1])
						<< " data[2] " << static_cast<int>(canPtr_->messageTx.data[2])
						<< " data[3] " << static_cast<int>(canPtr_->messageTx.data[3])
						<< " data[4] " << static_cast<int>(canPtr_->messageTx.data[4])
						<< " data[5] " << static_cast<int>(canPtr_->messageTx.data[5])
						<< " data[6] " << static_cast<int>(canPtr_->messageTx.data[6])
						<< " data[7] " << static_cast<int>(canPtr_->messageTx.data[7]);
				}
				canPtr_->sendMessage();
				//BOOST_LOG(logger_) << "INF " << "Router::setupModule: Message sent";
				//assert(1 != 1);
				delete result;
				break;
			}
		}
	}
	BOOST_LOG(logger_) << "INF " << "Router::setupModule: done";
	return;	
}

