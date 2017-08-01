#include "stdafx.h"
#include "ProtocolManager.h"


ProtocolManager::ProtocolManager(EQM* eqmPtr) : eqmPtr_(eqmPtr)
{
}


ProtocolManager::~ProtocolManager()
{
}

CMESSAGE::CMessage::EProtocol ProtocolManager::getProtocol(CAN::messageCAN message)
{
	return static_cast<CMESSAGE::CMessage::EProtocol>(message.data[0]);
}

CMESSAGE::CMessage* ProtocolManager::createMessage(CAN::messageCAN message)
{
	int proto = message.data[0];
	std::cout << "ProtocolManager::createMessage " << proto << std::endl;
	switch (proto)
	{
	case 0:
		return new CMESSAGE::CInitialMessage(&message);
	case 1:
		return new CMESSAGE::CSimpleMessage(&message);
	case 2:
		return new CMESSAGE::CExtendedMessage(&message);
	case 3:
		return new CMESSAGE::CAuthorizedMessage(&message);
	case 4:
		return new CMESSAGE::CExtendedAuthorizedMessage(&message);
	case 5:
		return new CMESSAGE::CBigDataMessage(&message);
	case 6:
		return new CMESSAGE::CMaskMessage(&message);
	
	default:
		return nullptr;
	}
}

CAN::messageCAN ProtocolManager::createMessage(CMESSAGE::CMessage* message)
{
	switch (message->protocol)
	{
	case CMESSAGE::CMessage::EProtocol::CInitialProtocol:
		return prepareCANMessageForInitial(message);
	case CMESSAGE::CMessage::EProtocol::CSimpleProtocol:
		return prepareCANMessageForSimple(message);
	case CMESSAGE::CMessage::EProtocol::CExtendedProtocol:
		return prepareCANMessageForExtended(message);
	case CMESSAGE::CMessage::EProtocol::CMaskProtocol:
		return prepareCANMessageForMask(message);
	}
}

CAN::messageCAN ProtocolManager::prepareCANMessageForInitial(CMESSAGE::CMessage* message)
{
	std::cout << "ProtocolManager::prepareCANMessageForInitial " << std::endl;
	CMESSAGE::CInitialMessage* msg = static_cast<CMESSAGE::CInitialMessage*>(message);
	CAN::messageCAN cMsg;
	cMsg.id = std::stoi(msg->toDomain);
	cMsg.data[0] = static_cast<int>(msg->protocol);
	cMsg.data[1] = 100;
	cMsg.data[2] = msg->header;
	if (msg->optional1 != 0)
		cMsg.data[3] = msg->optional1;
	else
		cMsg.data[3] = 0;
	if (msg->optional2 != 0)
		cMsg.data[4] = msg->optional2;
	else
		cMsg.data[4] = 0;
	for (int i = 5; i < 8; i++)
		cMsg.data[i] = 0;
	return cMsg;
}
CAN::messageCAN ProtocolManager::prepareCANMessageForSimple(CMESSAGE::CMessage* message)
{
	CMESSAGE::CSimpleMessage* msg = static_cast<CMESSAGE::CSimpleMessage*>(message);
	CAN::messageCAN cMsg;
	cMsg.id = std::stoi(msg->toDomain);
	cMsg.data[0] = static_cast<int>(msg->protocol);
	cMsg.data[1] = 100;
	cMsg.data[2] = msg->header;
	cMsg.data[3] = msg->port;
	cMsg.data[4] = msg->value;
	for (int i = 5; i < 8; i++)
		cMsg.data[i] = 0;
	return cMsg;
}
CAN::messageCAN ProtocolManager::prepareCANMessageForExtended(CMESSAGE::CMessage* message)
{
	CMESSAGE::CExtendedMessage* msg = static_cast<CMESSAGE::CExtendedMessage*>(message);
	CAN::messageCAN cMsg;
	cMsg.id = std::stoi(msg->toDomain);
	cMsg.data[0] = static_cast<int>(msg->protocol);
	cMsg.data[1] = 100;
	cMsg.data[2] = msg->header;
	cMsg.data[3] = msg->port;
	cMsg.data[4] = msg->value;
	cMsg.data[5] = msg->additional;
	for (int i = 6; i < 8; i++)
		cMsg.data[i] = 0;
	return cMsg;
}
CAN::messageCAN ProtocolManager::prepareCANMessageForAuthorized(CMESSAGE::CMessage* message)
{
	CMESSAGE::CAuthorizedMessage* msg = static_cast<CMESSAGE::CAuthorizedMessage*>(message);
	CAN::messageCAN cMsg;
	cMsg.id = std::stoi(msg->toDomain);
	cMsg.data[0] = static_cast<int>(msg->protocol);
	cMsg.data[1] = 100;
	cMsg.data[2] = msg->header;
	cMsg.data[3] = msg->login;
	cMsg.data[4] = msg->value;
	cMsg.data[5] = msg->additional;
	for (int i = 6; i < 8; i++)
		cMsg.data[i] = 0;
	return cMsg;
}
CAN::messageCAN ProtocolManager::prepareCANMessageForExtAuthorized(CMESSAGE::CMessage* message)
{
	CMESSAGE::CExtendedAuthorizedMessage* msg = static_cast<CMESSAGE::CExtendedAuthorizedMessage*>(message);
	CAN::messageCAN cMsg;
	cMsg.id = std::stoi(msg->toDomain);
	cMsg.data[0] = static_cast<int>(msg->protocol);
	cMsg.data[1] = 100;
	cMsg.data[2] = msg->header;
	cMsg.data[3] = msg->login;
	cMsg.data[4] = msg->pass;
	cMsg.data[5] = msg->value;
	cMsg.data[6] = msg->additional;
	cMsg.data[7] = 0;
	return cMsg;
}
CAN::messageCAN ProtocolManager::prepareCANMessageForBigData(CMESSAGE::CMessage* message)
{
	CMESSAGE::CBigDataMessage* msg = static_cast<CMESSAGE::CBigDataMessage*>(message);
	CAN::messageCAN cMsg;
	cMsg.id = std::stoi(msg->toDomain);
	cMsg.data[0] = static_cast<int>(msg->protocol);
	cMsg.data[1] = 100;
	cMsg.data[2] = msg->header;
	cMsg.data[3] = msg->wordSize;
	cMsg.data[4] = msg->data[0];
	cMsg.data[5] = msg->data[1];
	cMsg.data[6] = msg->data[2];
	cMsg.data[7] = msg->data[3];
	return cMsg;
}
CAN::messageCAN ProtocolManager::prepareCANMessageForMask(CMESSAGE::CMessage* message)
{
	CMESSAGE::CMaskMessage* msg = static_cast<CMESSAGE::CMaskMessage*>(message);
	CAN::messageCAN cMsg;
	cMsg.id = std::stoi(msg->toDomain);
	cMsg.data[0] = static_cast<int>(msg->protocol);
	cMsg.data[1] = 100;
	cMsg.data[2] = msg->header;
	cMsg.data[3] = msg->mask1;
	cMsg.data[4] = msg->mask2;
	for (int i = 5; i < 8; i++)
		cMsg.data[i] = 0;
	return cMsg;
}



CAN::messageCAN ProtocolManager::createProtocolNegotatorMessage(int protocol, std::string domain)
{
	CMESSAGE::CInitialMessage* initMsg = new CMESSAGE::CInitialMessage();
	initMsg->protocol = CMESSAGE::CMessage::EProtocol::CInitialProtocol;
	initMsg->toDomain = domain;
	initMsg->fromDomain = 100;
	initMsg->header = BC;
	initMsg->optional1 = 0;
	initMsg->optional2 = protocol;
	return createMessage(initMsg);
}