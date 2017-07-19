#pragma once
#include <vector>
#include <string>
#include "Objects\Obj.hpp"
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include "MODULE.hpp"
#include <functional>
#include <utility>
#include <fstream>
#include <boost\random\mersenne_twister.hpp>
#include <boost\random\uniform_int_distribution.hpp>

namespace CACHE
{
	class Subscription
	{
	public:
		Subscription(std::string name, std::function<void(Obj*)> func) { this->name = name; this->func = func; };
		std::string name;
		std::function<void(Obj*)> func;
		int subscriptionId;
	};
}

class Cache
{
public:
	Cache(boost::log::sources::logger_mt logger);
	~Cache();
	int addObject(Obj* obj);
	int removeObj(Obj* obj);
	void removeFromChild(Obj* parent, Obj* child);
	int addToChildren(Obj* parent, Obj* child);
	void commitChanges(std::string name);
	std::vector<Obj*> getAllObjects(std::string name);
	std::vector<Obj*> getAllObjectsFromChildren(std::string parentName, std::string name);
	std::vector<Obj*> getAllObjectsUnder(Obj* object, std::string name);
	std::vector<Obj*> getAllObjectsFromGrandChildren(std::string grandName, std::string parentName, std::string name);
	Obj* getUniqueObject(std::string name);
	Obj* getUniqueObjectUnder(Obj* obj, std::string);
	Obj* getUniqueObjectFromChildren(std::string parentName, std::string childName);
	Obj* getUniqueObjectFromGrandChildren(std::string grandName, std::string parentName, std::string childName);
	int subscribe(std::string name, std::function<void(Obj*)> func);
	//int subscribe(std::string name, std::function<void()> func);
	void unsubscribe(int subscriptionId);
private:
	std::vector<Obj*> cache_;
	boost::log::sources::logger_mt logger_;
	std::vector<CACHE::Subscription*> subrsciptions_;
	boost::random::mt19937 gen_;

	void checkAndRunSubscription(Obj* obj);
	void checkAndRunSubscription(std::string name);
	void dumpObject(std::string serialized);
	
};

