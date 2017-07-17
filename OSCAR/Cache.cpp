#include "stdafx.h"
#include "Cache.hpp"
#include <iostream>

Cache::Cache(boost::log::sources::logger_mt logger) : logger_(logger)
{
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
		return 0;
	}
	BOOST_LOG(logger_) << "WRN " << "Cache::removeObj: object " << obj << " does not exist";
	return 1;
}

std::vector<Obj*> Cache::getAllObjects(std::string name)
{
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
	for (const auto &obj : cache_)
	{
		if (obj->name == name)
		{
			return obj;
		}
	}
	return nullptr;
}

Obj* Cache::getUniqueObjectFromChildren(std::string parentName, std::string childName)
{
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

std::vector<Obj*> Cache::getAllObjectsFromGrandChildren(std::string grandName, std::string parentName, std::string name)
{
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
