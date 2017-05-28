#include "stdafx.h"
#include "UIA.hpp"


UIA::UIA(std::string domain, boost::log::sources::logger_mt logger)
{
	logger_ = logger;
	this->domain = domain;
	name = "UIA";
	BOOST_LOG(logger_) << "DBG " << "UIA ctor";
}


UIA::~UIA()
{
}

void UIA::initialize(std::string subcomponent)
{
	
}

void UIA::execute(std::string message)
{
	BOOST_LOG(logger_) << "DBG " << "UIA::execute " << message;
	/*
	0x0401yyyy - SAVE_USER_NAME; yyyy - name
	0x0402 - SAVE_CHAIR_POS
	0x0403 - SAVE_MIRROR_POS
	*/
	if (message.find("0x0401") != std::string::npos)
	{
		BOOST_LOG(logger_) << "INFO " << "UIA::execute: " << "SAVE_USER_NAME " << message.substr(6, message.size() - 6);
		userObjPtr_->username = message.substr(6, message.size() - 6);
		sendToUI("SAVE_OPERATION_RESULT_OK");
	}
	else if (message == "0x0402")
	{
		BOOST_LOG(logger_) << "INFO " << "UIA::execute: " << "SAVE_CHAIR_POS";
		sendToUI("SAVE_OPERATION_RESULT_OK");
	}
	else if (message == "0x0403")
	{
		BOOST_LOG(logger_) << "INFO " << "UIA::execute: " << "SAVE_MIRROR_POS";
		sendToUI("SAVE_OPERATION_RESULT_OK");
	}
}

void UIA::execute(INTER_MODULE_OPERATION* imo)
{
	//0x0401 - new user - should start wizard
	if (userObjPtr_ == nullptr)
	{
		getUserObject();
	}
	BOOST_LOG(logger_) << "DBG " << "UIA::execute " << imo->operation;
	if (imo->operation == "NEW_USER")
	{
		sendToUI("0x0401");
	}
}

void UIA::sendToUI(std::string message)
{
	std::fstream uiStream(uiSendPath_, std::ios::out);
	uiStream << message;
	uiStream.close();
	BOOST_LOG(logger_) << "DBG: " << "UIA::sendToUI " << message << " sent to " << uiSendPath_;
}

void UIA::getUserObject()
{
	for (const auto &obj : *cache_)
	{
		if (obj->name == "USER")
		{
			userObjPtr_ = static_cast<USER*>(obj);
			return;
		}
			

	}
}
