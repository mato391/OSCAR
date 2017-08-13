#include "stdafx.h"
#include "Cache.hpp"
#include <iostream>

Cache::Cache(boost::log::sources::logger_mt logger) : logger_(logger)
{
	std::fstream file("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Logs\\CacheDump.txt", std::ios::out);
	file << "";
	file.close();
	tmp_ = 0;
	//funcTemp_ = std::bind(&Cache::emptyFunction, this, std::placeholders::_1);
}


Cache::~Cache()
{
}

int Cache::addObject(Obj* obj)
{
	BOOST_LOG(logger_) << "INF " << "Cache::addObject: adding object " << obj << " name: " << obj->name;
	for (const auto &object : cache_)
	{
		if (object == obj)
		{
			BOOST_LOG(logger_) << "WRN " << "Cache::addObject: object " << obj << " already exist";
			return 1;
		}
	}
	cache_.push_back(obj);	
	checkAndRunSubscription(obj, CACHE::Subscription::EType::created);
	return 0;
}

int Cache::removeObj(Obj* obj)
{
	BOOST_LOG(logger_) << "INF " << "Cache::removeObj: removing object " << obj << " name: " << obj->name;
	std::vector<Obj*>::iterator iterForObjToRemove = cache_.end() + 1;
	for (std::vector<Obj*>::iterator i = cache_.begin(); i != cache_.end(); i++)
	{
		if ((*i) == obj)
		{
			iterForObjToRemove = i;
		}
	}
	if (iterForObjToRemove != cache_.end() + 1)
	{
		cache_.erase(iterForObjToRemove);
		checkAndRunSubscription(obj, CACHE::Subscription::EType::removed);
		return 0;
	}
	BOOST_LOG(logger_) << "WRN " << "Cache::removeObj: object " << obj << " does not exist";
	return 1;
}

void Cache::removeFromChild(Obj* parent, Obj* child)
{
	BOOST_LOG(logger_) << "INF " << "Cache::removeObj: removing child " << child << " name: " << child->name;
	std::vector<Obj*>::iterator iter = parent->children.end();
	for(std::vector<Obj*>::iterator i = parent->children.begin(); i != parent->children.end(); i++)
	{
		if ((*i) == child)
		{
			iter = i;
			break;
		}
	}
	if (iter != parent->children.end())
	{
		parent->children.erase(iter);
		return;
	}
	BOOST_LOG(logger_) << "WRN " << "Cache::removeObj: object " << child->name << " does not exist";
		
}

int Cache::addToChildren(Obj* parent, Obj* child)
{
	BOOST_LOG(logger_) << "INF " << "Cache::addToChildren: adding child " << child << " " << child->name << " to " << parent << " " << parent->name;
	for (const auto &childObj : parent->children)
	{
		if (childObj == child)
		{
			BOOST_LOG(logger_) << "WRN " << "Cache::addToChildren: Child exists " << childObj << " " << childObj->name;
			return 1;
		}
	}
	parent->children.push_back(child);
	checkAndRunSubscription(child, CACHE::Subscription::EType::created);
	return 0;
}

void Cache::commitChanges(Obj* obj)
{
	//BOOST_LOG(logger_) << "DBG " << "Cache::commitChanges " << obj->name;
	checkAndRunSubscription(obj, CACHE::Subscription::EType::updated);
}

std::vector<Obj*> Cache::getAllObjects(std::string name)
{
	BOOST_LOG(logger_) << "INF " << "Cache::getAllObjects " << name;
	std::vector<Obj*> objVec;
	for (const auto &obj : cache_)
	{
		if (obj->name == name)
		{
			objVec.push_back(obj);
		}
	}
	return objVec;
}

std::vector<Obj> Cache::getObjects(std::string name)
{
	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " " << name;
	std::vector<Obj> objVec;
	for (const auto &obj : cache_)
	{
		if (obj->name == name)
		{
			objVec.push_back(*obj);
		}
	}
	return objVec;
}
Obj* Cache::searchObject(std::string name)
{
	for (const auto &obj : cache_)
	{
		if (obj->name == name)
			return obj;
		else
			return searchInChildren(obj, name);
			
	}
}

Obj* Cache::searchInChildren(Obj* obj, std::string name)
{
	if (!obj->children.empty())
	{
		for (const auto &child : obj->children)
		{
			if (child->name == name)
				return child;
			else
				return searchInChildren(child, name);
		}
	}
	
}

Obj* Cache::getUniqueObject(std::string name)
{
	BOOST_LOG(logger_) << "INF " << "Cache::getUniqueObject " << name;
	for (const auto &obj : cache_)
	{
		if (obj->name == name)
		{
			return obj;
		}
	}
	return nullptr;
}

Obj* Cache::getUniqueObjectUnder(Obj* obj, std::string name)
{
	for (const auto &child : obj->children)
	{
		if (child->name == name)
		{
			return child;
		}
	}
	return nullptr;
}

Obj* Cache::getUniqueObjectFromChildren(std::string parentName, std::string childName)
{
	BOOST_LOG(logger_) << "INF " << "Cache::getUniqueObjectFromChildren parentName: " << parentName << " childName " << childName;
	auto obj = getUniqueObject(parentName);
	if (obj != nullptr)
	{
		for (const auto &child : obj->children)
		{
			if (child->name == childName)
			{
				return child;
			}
		}
	}
	return nullptr;
}

Obj* Cache::getUniqueObjectFromGrandChildren(std::string grandName, std::string parentName, std::string childName)
{
	BOOST_LOG(logger_) << "INF " << "Cache::getUniqueObjectFromGrandChildren grandName: " << grandName << " parentName: " << parentName << " childName " << childName;
	auto obj = getUniqueObjectFromChildren(grandName, parentName);
	if (obj != nullptr)
	{
		for (const auto &child : obj->children)
		{
			if (child->name == childName)
			{
				return child;
			}
		}
	}
	return nullptr;
}

std::vector<Obj*> Cache::getAllObjectsFromChildren(std::string parentName, std::string name)
{
	BOOST_LOG(logger_) << "INF " << "Cache::getAllObjectsFromChildren parentName: " << parentName << " childName " << name;
	auto object = getUniqueObject(parentName);
	std::vector<Obj*> objVec;
	if (object != nullptr && !object->children.empty())
	{
		for (const auto &obj : object->children)
		{
			if (obj->name == name)
			{
				objVec.push_back(obj);
			}
		}
		return objVec;
	}
	return{};
}

std::vector<Obj*> Cache::getAllObjectsUnder(Obj* object, std::string name)
{
	BOOST_LOG(logger_) << "INF " << "Cache::getAllObjectsFromChildren parentName: " << object << " name " << object->name << " childName " << name;
	std::vector<Obj*> objVec;
	for (const auto &obj : object->children)
	{
		if (obj->name == name)
		{
			objVec.push_back(obj);
		}
	}
	return objVec;
}

std::vector<Obj*> Cache::getAllObjectsFromGrandChildren(std::string grandName, std::string parentName, std::string name)
{
	BOOST_LOG(logger_) << "INF " << "Cache::getAllObjectsFromGrandChildren grandName: " << grandName << " parentName: " << parentName << " childName " << name;
	auto object = getUniqueObjectFromChildren(grandName, parentName);
	std::vector<Obj*> objVec;
	if (object != nullptr && !object->children.empty())
	{
		for (const auto &obj : object->children)
		{
			if (obj->name == name)
			{
				objVec.push_back(obj);
			}
		}
		return objVec;
	}
	return {};
}

std::vector<int> Cache::subscribe(std::string name, std::function<void(Obj*)> func, std::vector<int> types)
{
	std::vector<int> subIds;
	for (const auto &type : types)
	{
		auto sub = new CACHE::Subscription(name, func);
		boost::random::uniform_int_distribution<> dist(10, 1000);
		sub->subscriptionId = dist(gen_);
		sub->type = static_cast<CACHE::Subscription::EType>(type);
		subIds.push_back(sub->subscriptionId);
		if (sub->type == CACHE::Subscription::EType::created)
			subrsciptionsForCreate_.push_back(sub);
		else if (sub->type == CACHE::Subscription::EType::updated)
			subrsciptionsForUpdate_.push_back(sub);
		else if (sub->type == CACHE::Subscription::EType::removed)
			subrsciptionsForRemove_.push_back(sub);
		else
			BOOST_LOG(logger_) << "WRN " << __FUNCTION__ << "subscription type unknown";
		BOOST_LOG(logger_) << "INF " << __FUNCTION__ << ": subscribed to: " << name << " id " << sub->subscriptionId;
		checkAndRunSubscription(name, static_cast<CACHE::Subscription::EType>(type));	//fetch should be added to interface
	}
	//showSubscriptions();
	return subIds;
}

void Cache::showSubscriptions()
{
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " subscriptions for create size: " << subrsciptionsForCreate_.size();
	for (const auto &sub : subrsciptionsForCreate_)
	{
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " subId: " << sub->subscriptionId
			<< " objName: " << sub->name;
	}
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " subscriptions for update size: " << subrsciptionsForUpdate_.size();
	for (const auto &sub : subrsciptionsForUpdate_)
	{
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " subId: " << sub->subscriptionId
			<< " objName: " << sub->name;
	}
	BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " subscriptions for remove size: " << subrsciptionsForRemove_.size();
	for (const auto &sub : subrsciptionsForRemove_)
	{
		BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " subId: " << sub->subscriptionId
			<< " objName: " << sub->name;
	}
}

void Cache::dumpObject(std::string serialized)
{
	std::fstream file("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Logs\\CacheDump.txt", std::ios::app);
	file << serialized << std::endl;
	file.close();
}

void Cache::checkAndRunSubscription(Obj* obj, CACHE::Subscription::EType type)
{
	//BOOST_LOG(logger_) << "INF " << "Cache::checkAndRunSubscription: for object: " << obj->name;
	std::vector<CACHE::Subscription*> subrsciptions;
	if (type == CACHE::Subscription::EType::created)
		subrsciptions = subrsciptionsForCreate_;
	else if (type == CACHE::Subscription::EType::updated)
		subrsciptions = subrsciptionsForUpdate_;
	else if (type == CACHE::Subscription::EType::removed)
		subrsciptions = subrsciptionsForRemove_;
	//BOOST_LOG(logger_) << "DBG " << __FUNCTION__ << " subscriptions size: " << subrsciptions.size();
	for (const auto &sub : subrsciptions)
	{
		if (sub->name == obj->name && sub->type == type)
		{
			BOOST_LOG(logger_) << "INF " << "Cache::checkAndRunSubscription: run sub: " << sub->subscriptionId  << " " << sub->name << " " << static_cast<int>(sub->type);
			subTmp_ = sub;
			objTmp_ = obj;
			boost::thread t(std::bind(&Cache::runOnObject, this, sub, obj));
			t.detach();
		}
		//else
		//	BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " sub name is another or type is not the same " << sub->name << " " << static_cast<int>(sub->type);
	}
	//funcTemp_ = std::bind(&Cache::emptyFunction, this, std::placeholders::_1);
	//BOOST_LOG(logger_) << "DBG " << "Cache::checkAndRunSubscription: checkingDone ";
}

void Cache::checkAndRunSubscription(std::string name, CACHE::Subscription::EType type)
{
	//BOOST_LOG(logger_) << "INF " << "Cache::checkAndRunSubscription: for name: " << name;
	auto obj = getUniqueObject(name);
	if (obj != nullptr)
	{
		std::vector<CACHE::Subscription*> subrsciptions;
		if (type == CACHE::Subscription::EType::created)
			subrsciptions = subrsciptionsForCreate_;
		else if (type == CACHE::Subscription::EType::updated)
			subrsciptions = subrsciptionsForUpdate_;
		else if (type == CACHE::Subscription::EType::removed)
			subrsciptions = subrsciptionsForRemove_;
		for (const auto &sub : subrsciptions)
		{
			if (sub->name == name && sub->type == type)
			{
				BOOST_LOG(logger_) << "INF " << "Cache::checkAndRunSubscription: run sub: " << sub->subscriptionId << " " << sub->name << " " << static_cast<int>(sub->type);
				objTmp_ = obj;
				subTmp_ = sub;
				boost::thread t(std::bind(&Cache::runOnObject, this, sub, obj));
				t.detach();
			}
			//else
				//BOOST_LOG(logger_) << "INF " << __FUNCTION__ << " sub name is another or type is not the same "  << sub->name << " " << static_cast<int>(sub->type);
		}
		//funcTemp_ = std::bind(&Cache::emptyFunction, this, std::placeholders::_1);
	}
	
}

void Cache::runOnObject(CACHE::Subscription* sub, Obj* obj)
{
	BOOST_LOG(logger_) << "INF " << "Cache::runOnObject for subId: " << subTmp_->subscriptionId << "  in thread: " << boost::this_thread::get_id();
	if (subTmp_ != nullptr)
	{
		while (!mtx_.try_lock())
		{
			boost::this_thread::sleep(boost::posix_time::microseconds(200));
		}
		sub->func(obj);
		mtx_.unlock();
	}
	else
		BOOST_LOG(logger_) << "WRN " << __FUNCTION__ << " funcTemp is a nullptr";
		
}

void Cache::changeSubscriptionType(int subId, int type)
{
	std::vector<CACHE::Subscription*>* subrsciptions = nullptr;
	if (static_cast<CACHE::Subscription::EType>(type) == CACHE::Subscription::EType::created)
		subrsciptions = &subrsciptionsForCreate_;
	else if (static_cast<CACHE::Subscription::EType>(type) == CACHE::Subscription::EType::updated)
		subrsciptions = &subrsciptionsForUpdate_;
	else if (static_cast<CACHE::Subscription::EType>(type) == CACHE::Subscription::EType::removed)
		subrsciptions = &subrsciptionsForRemove_;
	std::vector<CACHE::Subscription*>::iterator iter;
	if (subrsciptions != nullptr)
	{
		for (iter = subrsciptions->begin(); iter != subrsciptions->end(); iter++)
		{
			if ((*iter)->subscriptionId == subId)
			{
				BOOST_LOG(logger_) << "INF " << "Cache::unsubscribe: unsubscribed to: " << (*iter)->name << " id " << (*iter)->subscriptionId;
				(*iter)->type = static_cast<CACHE::Subscription::EType>(type);
				break;
			}
		}
	}
}

void Cache::unsubscribe(int subscriptionId, int type)
{
	std::vector<CACHE::Subscription*>* subrsciptions = nullptr;
	if (static_cast<CACHE::Subscription::EType>(type) == CACHE::Subscription::EType::created)
		subrsciptions = &subrsciptionsForCreate_;
	else if (static_cast<CACHE::Subscription::EType>(type) == CACHE::Subscription::EType::updated)
		subrsciptions = &subrsciptionsForUpdate_;
	else if (static_cast<CACHE::Subscription::EType>(type) == CACHE::Subscription::EType::removed)
		subrsciptions = &subrsciptionsForRemove_;
	std::vector<CACHE::Subscription*>::iterator iter;
	if (subrsciptions != nullptr)
	{
		for (iter = subrsciptions->begin(); iter != subrsciptions->end(); iter++)
		{
			if ((*iter)->subscriptionId == subscriptionId)
			{
				BOOST_LOG(logger_) << "INF " << "Cache::unsubscribe: unsubscribed to: " << (*iter)->name << " id " << (*iter)->subscriptionId;
				break;
			}
		}
		if ((*iter) != nullptr)
		{
			mtx_.lock();
			subrsciptions->erase(iter);
			mtx_.unlock();
		}
			
	}
	
}