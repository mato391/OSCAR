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
#include <boost\thread.hpp>
#include <boost\thread\mutex.hpp>
namespace CACHE
{
	class Subscription
	{
	public:
		Subscription(std::string name, std::function<void(Obj*)> func) { this->name = name; this->func = func; };
		std::string name;
		std::function<void(Obj*)> func;
		int subscriptionId;
		enum class EType
		{
			created = 0,
			updated = 1,
			removed = 2
		};
		EType type;
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
	void commitChanges(Obj* obj);
	std::vector<Obj*> getAllObjects(std::string name);
	std::vector<Obj> getObjects(std::string name);
	std::vector<Obj*> getAllObjectsFromChildren(std::string parentName, std::string name);
	std::vector<Obj*> getAllObjectsUnder(Obj* object, std::string name);
	std::vector<Obj*> getAllObjectsFromGrandChildren(std::string grandName, std::string parentName, std::string name);
	Obj* getUniqueObject(std::string name);
	Obj* getUniqueObjectUnder(Obj* obj, std::string);
	Obj* getUniqueObjectFromChildren(std::string parentName, std::string childName);
	Obj* getUniqueObjectFromGrandChildren(std::string grandName, std::string parentName, std::string childName);
	std::vector<int> subscribe(std::string name, std::function<void(Obj*)> func, std::vector<int> types);
	//int subscribe(std::string name, std::function<void()> func);
	void unsubscribe(int subscriptionId, int type);
	void changeSubscriptionType(int subId, int type);
	Obj* searchObject(std::string name);
private:
	std::vector<Obj*> cache_;
	boost::log::sources::logger_mt logger_;
	std::vector<CACHE::Subscription*> subrsciptionsForCreate_;
	std::vector<CACHE::Subscription*> subrsciptionsForUpdate_;
	std::vector<CACHE::Subscription*> subrsciptionsForRemove_;
	boost::random::mt19937 gen_;
	std::function<void(Obj*)> funcTemp_;
	Obj* objTmp_;
	std::string nameTmp_;
	boost::mutex mtx_;
	CACHE::Subscription* subTmp_;
	int tmp_;
	int levelCounter_;
	void checkAndRunSubscription(Obj* obj, CACHE::Subscription::EType type);
	void checkAndRunSubscription(std::string name, CACHE::Subscription::EType type);
	void dumpObject(std::string serialized);
	void runOnObject(CACHE::Subscription* sub, Obj* obj);
	void emptyFunction(Obj* obj) { BOOST_LOG(logger_) << "INF " << __FUNCTION__; };
	void showSubscriptions();
	Obj* searchInChildren(Obj* obj, std::string name);
};

