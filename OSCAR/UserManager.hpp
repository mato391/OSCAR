#pragma once
#include "USER.hpp"
#include <boost\filesystem.hpp>
#include <boost\algorithm\string.hpp>
class UserManager
{
public:
	UserManager();
	~UserManager();
	void setupUser(std::string sn);
	USER* getUser() { return userObj_; }
private:
	USER* userObj_;
	const std::string ucfFilePath = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\config\\UCF_";

	void createUCF(std::vector<std::string> sUcfContent);
	void parseUCF(std::string ucfFilePath);
	bool checkIfUCFExists(std::string sn);
};

