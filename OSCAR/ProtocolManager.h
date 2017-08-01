#pragma once
#include <vector>
#include "CAN.h"
#include "EQM.hpp"
#include "MODULE.hpp"
#include "CMessage.hpp"

#include <boost\log\trivial.hpp>
#include <boost\move\utility.hpp>
#include <boost\log\sources\logger.hpp>
#include <boost\optional.hpp>

#define BC 188
class ProtocolManager
{
public:
	ProtocolManager(EQM* eqmPtr);
	~ProtocolManager();
	CMESSAGE::CMessage::EProtocol getProtocol(CAN::messageCAN message);
	CMESSAGE::CMessage* createMessage(CAN::messageCAN message);
	CAN::messageCAN createMessage(CMESSAGE::CMessage* message);
	void setDomain();
	CAN::messageCAN createProtocolNegotatorMessage(int protocol, std::string domain);
private:
	EQM* eqmPtr_;
	CAN::messageCAN prepareCANMessageForInitial(CMESSAGE::CMessage* message);
	CAN::messageCAN prepareCANMessageForSimple(CMESSAGE::CMessage* message);
	CAN::messageCAN prepareCANMessageForExtended(CMESSAGE::CMessage* message);
	CAN::messageCAN prepareCANMessageForAuthorized(CMESSAGE::CMessage* message);
	CAN::messageCAN prepareCANMessageForExtAuthorized(CMESSAGE::CMessage* message);
	CAN::messageCAN prepareCANMessageForBigData(CMESSAGE::CMessage* message);
	CAN::messageCAN prepareCANMessageForMask(CMESSAGE::CMessage* message);;

};

