#include "stdafx.h"
#include "Router.hpp"
#include "ComponentFactory.hpp"

Router::Router(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger) : logger_(logger)
{
	cache_ = cache;
	mmfPath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\MMF.txt";
	startComponentService();
}


Router::~Router()
{
}

void Router::startComponentService()
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
		std::vector<std::string> componentAddress;
		//std::cout << "ROUTER:StartComponentService: mmf: " << line << std::endl;
		boost::split(componentAddress, line, boost::is_any_of(":"));
		startComponent(componentAddress[0], componentAddress[1]);
	}
}

void Router::startComponent(std::string name, std::string address)
{
	components_.push_back(ComponentFactory::createComponent(name, address, logger_));
	components_.back()->setCache(cache_);
	components_.back()->setComponentsCache(&components_);
	components_.back()->initialize();
	components_.back()->setSenderPtr(std::bind(&Router::sender, this, std::placeholders::_1));
}

void Router::receiver(std::string data)
{
	std::string domain = data.substr(0, 4);
	BOOST_LOG(logger_) << "INFO " << "Router::receiver: " << domain;
	for (const auto &component : components_)
	{
		if (component->domain == domain)
		{
			component->execute(data);
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
		BOOST_LOG(logger_) << "INFO " << "Router::sender: CAN sending signal: " << data << std::endl;
		//router->receiver(content);
	}
}