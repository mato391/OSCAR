#pragma once
#include <string>
#include <vector>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class Action
{
public:
	Action(std::vector<int> connIdsVec, std::vector<int> connValues, std::string name) { this->name = name; connIds = connIdsVec; this->connValues = connValues; }
	std::string name;
	std::vector<int> connIds;
	std::vector<int> connValues;
};

class ActionSet
{
public:
	ActionSet(boost::log::sources::logger_mt logger);
	~ActionSet();
	Action* getAction(std::string name);
	void addAction(std::vector<int> connsIds, std::vector<int> connValues, std::string name);
private:
	std::vector<Action*> actions_;
	boost::log::sources::logger_mt logger_;
};

