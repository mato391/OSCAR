#include "stdafx.h"
#include "BDM.hpp"


BDM::BDM(std::string domain, boost::log::sources::logger_mt logger) 
{
	configuringState = EConfiguringState::online;
	logger_ = logger;
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__;
	this->domain = domain;
	name = "BDM";
	resultSubId_ = 0;
}

BDM::~BDM()
{
}

RESULT* BDM::setup(int domain)
{
	if (resultSubId_ == 0)
		resultSubId_ = cachePtr->subscribe("RESULT", std::bind(&BDM::handleOperationResult, this, std::placeholders::_1), { 0 })[0];
	BOOST_LOG(logger_) << "INF " << "BDM::setup: " << domain;
	std::string sdomain = (domain < 10 ) ? "0x0"+std::to_string(domain) : "0x" + std::to_string(domain);
	if (sdomain == "0x05")
	{
		doorModule_->setup();
		RESULT* result = new RESULT();
		result->applicant = "BDM";
		result->feedback = std::to_string(doorModule_->getModuleProtocol());
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << result->feedback;
		result->status = RESULT::EStatus::success;
		return result;

	}
	else if (sdomain == "0x06")
	{
		lightModule_->setup();
		RESULT* result = new RESULT();
		result->applicant = "BDM";
		result->feedback = std::to_string(lightModule_->getModuleProtocol());
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << result->feedback;
		result->status = RESULT::EStatus::success;
		return result;
	}
	else if (sdomain == "0x09")
	{
		mirrorModule_->setup();
		RESULT* result = new RESULT();
		result->applicant = "BDM";
		result->feedback = std::to_string(mirrorModule_->getModuleProtocol());
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << result->feedback;
		result->status = RESULT::EStatus::success;
		return result;
	}
	return nullptr;
	//getResultAndSendToRouter();
}

void BDM::handleOperationResult(Obj* obj)
{
	auto doorMod = getModuleWithDomain(getDomainFor("BDM_DOOR"));
	auto lightMod = getModuleWithDomain(getDomainFor("BDM_LIGHT"));
	auto moduleTaskResult = static_cast<RESULT*>(obj);
	BOOST_LOG(logger_) << "INF " << "BDM::handleOperationResult " << moduleTaskResult->applicant;
	if (moduleTaskResult->feedback != "")
	{
		CMESSAGE::CMessage* msg = convertResultToCMessage(moduleTaskResult);
		send(msg);
	}
	if (moduleTaskResult->applicant == "LIGHT_MODULE" && lightMod != nullptr)
	{
		cachePtr->removeFromChild(lightMod, moduleTaskResult);
	}
	else
	{
		cachePtr->removeFromChild(doorMod, moduleTaskResult);
	}
}

CMESSAGE::CInitialMessage* BDM::initialProtocolMessageHandler(CMESSAGE::CMessage* msg)
{
	BOOST_LOG(logger_) << "INF " << "BDM::execute: "
		<< "initial message detected from domain "
		<< msg->fromDomain
		<< " bdmModules size: "
		<< bdmModules_.size();
	std::string moduleLabel;
	for (auto &module : bdmModules_)
	{
		BOOST_LOG(logger_) << "DBG " << "BDM::execute: " << "Module " << module.second->domain << " has been found";
		if (module.second->domain == msg->fromDomain)
		{
			module.second->detectionStatus = MODULE::EDetectionStatus::online;
			moduleLabel = module.first;
			BOOST_LOG(logger_) << "DBG " << "BDM::execute: " << "Module " << moduleLabel << " has been set";
			break;
		}
	}
	CMESSAGE::CInitialMessage* mesg = new CMESSAGE::CInitialMessage();
	mesg->header = AA;
	mesg->protocol = CMESSAGE::CMessage::EProtocol::CInitialProtocol;
	mesg->fromDomain = OWNID;
	mesg->toDomain = msg->fromDomain.substr(2, 2);
	mesg->optional1 = 0;
	mesg->optional2 = 0;
	return mesg;
}

CMESSAGE::CMessage* BDM::execute(CMESSAGE::CMessage* msg)
{
	BOOST_LOG(logger_) << "INF " << "BDM::execute " << msg->header;
	getBDMObjectIfNeeded();
	if (msg->getProtocol() == CMESSAGE::CMessage::EProtocol::CInitialProtocol
		&& msg->header == AA)
	{
		return initialProtocolMessageHandler(msg);
	}
	return nullptr;
}

CMESSAGE::CMessage* BDM::convertResultToCMessage(RESULT* res)
{
	if (res->applicant == "DOOR_MODULE")
	{
		if (res->feedback == "Initialize")
		{
			CMESSAGE::CInitialMessage* msg = new CMESSAGE::CInitialMessage();
			std::string tmpdomain = getDomainFor("BDM_DOOR");
			msg->toDomain = (tmpdomain.find("0x0") != std::string::npos) ? tmpdomain.substr(3, 1) : tmpdomain;
			BOOST_LOG(logger_) << "INF " << "BDM::convertResultToCMessage: msg->toDomain: " << msg->toDomain;
			msg->fromDomain = OWNID;
			msg->header = AA;
			msg->optional1 = 0;
			msg->optional2 = 0;
			msg->protocol = CMESSAGE::CMessage::EProtocol::CInitialProtocol;
			return msg;
		}
		else
		{
			CMESSAGE::CSimpleMessage* msg = new CMESSAGE::CSimpleMessage();
			std::string tmpdomain = getDomainFor("BDM_DOOR");
			msg->toDomain = (tmpdomain.find("0x0") != std::string::npos) ? tmpdomain.substr(3, 1) : tmpdomain;
			BOOST_LOG(logger_) << "INF " << "BDM::convertResultToCMessage: msg->toDomain: " << msg->toDomain;
			msg->fromDomain = OWNID;
			msg->header = CC;
			std::vector<std::string> tmp;
			boost::split(tmp, res->feedback, boost::is_any_of(":"));
			msg->port = std::stoi(tmp[0]);
			msg->value = std::stoi(tmp[1]);
			msg->protocol = CMESSAGE::CMessage::EProtocol::CSimpleProtocol;
			return msg;
		}
	}
	else if (res->applicant == "LIGHT_MODULE")
	{
		CMESSAGE::CExtendedMessage* msg = new CMESSAGE::CExtendedMessage();
		std::string tmpdomain = getDomainFor("BDM_LIGHT");
		msg->toDomain = (tmpdomain.find("0x0") != std::string::npos) ? tmpdomain.substr(3, 1) : tmpdomain;
		BOOST_LOG(logger_) << "INF " << "BDM::convertResultToCMessage: msg->toDomain: " << msg->toDomain;
		msg->fromDomain = OWNID;
		msg->header = CC;
		std::vector<std::string> tmp;
		boost::split(tmp, res->feedback, boost::is_any_of(":"));
		msg->port = std::stoi(tmp[0]);
		msg->value = std::stoi(tmp[1]);
		msg->additional = std::stoi(tmp[2]);
		msg->protocol = CMESSAGE::CMessage::EProtocol::CExtendedProtocol;
		if (BDM_DBG)
		{
			BOOST_LOG(logger_) << "DBG " << "BDM::convertResultToCMessage: "
				<< " MSG->protocol " << static_cast<int>(msg->protocol)
				<< " \nMSG->fromDomain " << msg->fromDomain
				<< " \nMSG->header " << msg->header
				<< " \nMSG->port " << msg->port
				<< " \nMSG->value " << msg->value
				<< " \nMSG->additional " << msg->additional;
		}
		
		return msg;
	}
	else if (res->applicant == "ELA")
	{
		CMESSAGE::CMaskExtendedMessage* msg = new CMESSAGE::CMaskExtendedMessage();
		std::string tmpdomain = getDomainFor("BDM_LIGHT");

		msg->toDomain = (tmpdomain.find("0x0") != std::string::npos) ? tmpdomain.substr(3, 1) : tmpdomain;
		BOOST_LOG(logger_) << "INF " << "BDM::convertResultToCMessage: msg->toDomain: " << msg->toDomain;
		msg->fromDomain = OWNID;
		msg->header = CC;
		std::vector<std::string> tmp;
		boost::split(tmp, res->feedback, boost::is_any_of(":"));
		msg->mask1 = std::stoi(tmp[0]);
		msg->mask2 = std::stoi(tmp[1]);
		msg->mask3 = std::stoi(tmp[2]);
		msg->mask4 = std::stoi(tmp[3]);
		msg->counter = std::stoi(tmp[4]);
		msg->interval = std::stoi(tmp[5]);
		msg->protocol = CMESSAGE::CMessage::EProtocol::CMaskExtendedProtocol;
		if (BDM_DBG)
		{
			BOOST_LOG(logger_) << "DBG " << "BDM::convertResultToCMessage: "
				<< " MSG->protocol " << static_cast<int>(msg->protocol)
				<< " \nMSG->fromDomain " << msg->fromDomain
				<< " \nMSG->header " << msg->header
				<< " \nMSG->mask1 " << msg->mask1
				<< " \nMSG->mask2 " << msg->mask2
				<< " \nMSG->counter " << msg->counter
				<< " \nMSG->interval " << msg->interval;
		}

		return msg;
	}
	else if (res->applicant == "MIRROR_MODULE")
	{
		auto msg = new CMESSAGE::CMaskMessage();
		std::string tmpdomain = getDomainFor("BDM_MIRROR");
		msg->toDomain = (tmpdomain.find("0x0") != std::string::npos) ? tmpdomain.substr(3, 1) : tmpdomain;
		msg->fromDomain = OWNID;
		msg->header = CC;
		std::vector<std::string> tmp;
		boost::split(tmp, res->feedback, boost::is_any_of(":"));
		msg->mask1 = std::stoi(tmp[0]);
		msg->mask2 = std::stoi(tmp[1]); 
		msg->mask3 = 0;
		msg->mask4 = 0;
		msg->protocol = CMESSAGE::CMessage::EProtocol::CMaskProtocol;
		if (BDM_DBG)
		{
			BOOST_LOG(logger_) << "DBG " << "BDM::convertResultToCMessage: "
				<< " MSG->protocol " << static_cast<int>(msg->protocol)
				<< " \nMSG->fromDomain " << msg->fromDomain
				<< " \nMSG->header " << msg->header
				<< " \nMSG->mask1 " << msg->mask1
				<< " \nMSG->mask2 " << msg->mask2;
		}
		return msg;
	}
	return nullptr;
}

std::string BDM::getDomainFor(std::string label)
{
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " for " << label;
	for (const auto &mod : bdmModules_)
	{
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " " << mod.second->label;
		if (mod.second->label == label)
		{
			BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " returning " << mod.second->domain;
			return mod.second->domain;
		}
	}
	return "";
}

MODULE* BDM::getModuleWithDomain(std::string domain)
{
	for (const auto &mod : bdmModules_)
	{
		if (mod.second->domain == domain)
		{
			return mod.second;
		}
	}
	return nullptr;
}

void BDM::execute(std::string message)
{
	
}

void BDM::execute(INTER_MODULE_OPERATION* imo)
{

}

void BDM::initialize()
{
	doorModule_ = new DoorModule(cache_, logger_, cachePtr);
	doorModule_->initialize();
	lightModule_ = new LightModule(cache_, logger_, cachePtr);
	lightModule_->initialize();
	mirrorModule_ = new MirrorModule(cache_, logger_, cachePtr);
	mirrorModule_->initialize();
	setConfiguringStateIfNeeded();
	displayTopology();
}

void BDM::getBDMObjectIfNeeded()
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__;
	if (bdmModules_.empty() || bdmModules_.size() < 2)
	{
		auto eqmObj = static_cast<EQM*>(cachePtr->getUniqueObject("EQM"));
		if (eqmObj != nullptr)
		{

			for (const auto &mod : eqmObj->children)
			{
				auto module = static_cast<MODULE*>(mod);
				if (module->label.find("BDM") != std::string::npos)
				{
					BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " setting module " << module->label;
					bdmModules_[module->label] = module;
				}
			}
		}
	}
}

void BDM::setConfiguringStateIfNeeded()
{
	if (lightModule_ != nullptr && doorModule_ != nullptr && mirrorModule_ != nullptr)	//comments should be deleted
	{
		BOOST_LOG(logger_) << "INF " << "BDM::setConfiguringStateIfNeeded: " << "OK";
		configuringState = EConfiguringState::configured;
	}
}

void BDM::displayTopology()
{
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__;
	for (const auto &obj : bdmModules_)
	{
		BOOST_LOG(logger_) << obj.first;
		for (const auto &element : obj.second->children)
		{
			BOOST_LOG(logger_) << element->name;
			if (element->name == "CONNECTOR")
			{
				auto conn = static_cast<CONNECTOR*>(element);
				BOOST_LOG(logger_) << conn->id << " " << conn->label << " " << static_cast<int>(conn->type);
			}
			else if (element->name == "DOORS")
			{
				BOOST_LOG(logger_) << "CommonGND " << static_cast<DOORS*>(element)->commonLockGND->id;
				for (const auto &door : element->children)
				{
					auto doorC = static_cast<DOOR*>(element);
					BOOST_LOG(logger_) << doorC->label;
				}
			}

		}
	}
}