#pragma once
#include "Component.hpp"
#include <iostream>
#include <boost\algorithm\string.hpp>
#include "Objects\CP.hpp"
#include "Objects\DOOR.hpp"
#include "DOORS.hpp"
#include "ALARM.hpp"

#include <boost\algorithm\string.hpp>
#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>

class DoorModule 
{
public:
	DoorModule(std::vector<Obj*>* cache, boost::log::sources::logger_mt logger);
	~DoorModule();
	void unlockDoors();
	void lockDoors();
	void openWindow(std::string port);
	void closeWindow(std::string port);
	void lockWindow();
	void unlockWindow();
	void changeOpeningState(std::string port, DOOR::EOpeningState state);
	void initialize();
private:
	boost::log::sources::logger_mt logger_;
	CP* cpObj_;
	std::vector<Obj*>* cache_;
	DOORS* doorsObj_;
	std::vector<std::string> door6Labels_ = { "FRONT_LEFT", "FRONT_RIGHT", "BACK_LEFT", "BACK_RIGHT", "MASK", "BOOT" };
	std::vector<std::string> door5Labels_ = { "FRONT_LEFT", "FRONT_RIGHT", "MASK", "BOOT" ,"ROOF" };
	std::vector<std::string> door4Labels_ = { "FRONT_LEFT", "FRONT_RIGHT", "MASK", "BOOT" };

	void getCP();
	void prepareTopology();
	void displayTopology();
	std::string* checkIfDoorsAreClosed();
	bool checkIfBateryAlarmRaised();
	
	
};

