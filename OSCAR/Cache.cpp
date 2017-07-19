#include "stdafx.h"
#include "Cache.hpp"
#include <iostream>

Cache::Cache(boost::log::sources::logger_mt logger) : logger_(logger)
{
	std::fstream file("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Logs\\CacheDump.txt", std::ios::out);
	file << "";
	file.close();
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
	checkAndRunSubscription(obj);
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
		checkAndRunSubscription(obj);
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
	checkAndRunSubscription(child);
	return 0;
}

void Cache::commitChanges(std::string name)
{
	BOOST_LOG(logger_) << "DBG " << "Cache::commitChanges " << name;
	checkAndRunSubscription(name);
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

int Cache::subscribe(std::string name, std::function<void(Obj*)> func)
{
	auto sub = new CACHE::Subscription(name, func);
	boost::random::uniform_int_distribution<> dist(1, 10000);
	sub->subscriptionId = dist(gen_);
	subrsciptions_.push_back(sub);
	BOOST_LOG(logger_) << "INF " << "Cache::subscribe: subscribed to: " << name << " id " << sub->subscriptionId;
	checkAndRunSubscription(name);
	return sub->subscriptionId;
}

void Cache::dumpObject(std::string serialized)
{
	std::fstream file("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\Logs\\CacheDump.txt", std::ios::app);
	file << serialized << std::endl;
	file.close();
}

void Cache::checkAndRunSubscription(Obj* obj)
{
	BOOST_LOG(logger_) << "INF " << "Cache::checkAndRunSubscription: for object: " << obj->name;
	BOOST_LOG(logger_) << "DBG " << "Cache::checkAndRunSubscription: run sub.size(): " << subrsciptions_.size();
	for (const auto &sub : subrsciptions_)
	{
		if (sub->name == obj->name)
		{
			BOOST_LOG(logger_) << "INF " << "Cache::checkAndRunSubscription: run sub: " << sub->subscriptionId;
			sub->func(obj);
		}
	}
}

void Cache::checkAndRunSubscription(std::string name)
{
	BOOST_LOG(logger_) << "INF " << "Cache::checkAndRunSubscription: for object: " << name;
	auto obj = getUniqueObject(name);
	for (const auto &sub : subrsciptions_)
	{
		if (sub->name == name && obj != nullptr)
		{
			BOOST_LOG(logger_) << "INF " << "Cache::checkAndRunSubscription: run sub: " << sub->subscriptionId;
			sub->func(obj);
		}
	}
}

void Cache::unsubscribe(int subscriptionId)
{
	std::vector<CACHE::Subscription*>::iterator iter;
	for (iter = subrsciptions_.begin(); iter != subrsciptions_.end(); iter++)
	{
		if ((*iter)->subscriptionId == subscriptionId)
		{
			BOOST_LOG(logger_) << "INF " << "Cache::unsubscribe: unsubscribed to: " << (*iter)->name << " id " << (*iter)->subscriptionId;
			break;
		}
	}
	subrsciptions_.erase(iter);
	
}