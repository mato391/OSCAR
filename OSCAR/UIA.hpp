#pragma once
#include "Component.hpp"
#include <fstream>
#include <boost\filesystem.hpp>
#include "ControlsManager.hpp"
#include "USER.hpp"

class UIA :
	public Component
{
public:
	UIA(std::string domain, boost::log::sources::logger_mt logger);
	~UIA();
	void execute(std::string message);
	void execute(INTER_MODULE_OPERATION* imo);
	CMESSAGE::CMessage* execute(CMESSAGE::CMessage* msg) { return nullptr; };
	void setCache(std::vector<Obj*>* cache) { cache_ = cache; }
	void setComponentsCache(std::vector<Component*>* cache) { componentCache_ = cache; }
	void setSenderPtr(std::function<void(CMESSAGE::CMessage*)> func) { send = func; }
	void setup(std::string domain) {}
	RESULT* setup(int domain) { return new RESULT(); }
	void initialize();
private:
	std::vector<Obj*>* cache_;
	std::vector<Component*>* componentCache_;
	USER* userObjPtr_;
	ControlsManager* conMan_;
	const std::string uiRecvPath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\ui_send.txt";
	const std::string uiSendPath_ = "D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\ui_recv.txt";

	void sendToUI(std::string message);
	void getUserObject();
};

