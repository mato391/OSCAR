#include "stdafx.h"
#include "UserManager.hpp"


UserManager::UserManager()
{
}


UserManager::~UserManager()
{
}

void UserManager::setupUser(std::string sn)
{
	userObj_ = new USER();
	userObj_->username = "";
	userObj_->rcSerialCode = sn;
	if (checkIfUCFExists(sn))
	{
		parseUCF(ucfFilePath + sn + ".txt");
		userObj_->isNew = false;
	}
	else
	{
		userObj_->isNew = true;
	}
}

bool UserManager::checkIfUCFExists(std::string sn)
{
	if (boost::filesystem::exists(ucfFilePath + sn + ".txt"))
		return true;
	else
		return false;
}

void UserManager::parseUCF(std::string ucfFilePath)
{
	std::fstream ucfFile(ucfFilePath, std::ios::in);
	std::string ucfContent;
	ucfFile >> ucfContent;
	ucfFile.close();
	std::vector<std::string> sUcfContent;
	boost::split(sUcfContent, ucfContent, boost::is_any_of(";"));
	createUCF(sUcfContent);
}

void UserManager::createUCF(std::vector<std::string> sUcfContent)
{


}