#include "stdafx.h"
#include "ProtocolManager.h"


ProtocolManager::ProtocolManager()
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
	switch (proto)
	{
	case 0:
		return static_cast<CMESSAGE::CMessage*>(&CMESSAGE::CInitialMessage(&message));
	case 1:
		return static_cast<CMESSAGE::CMessage*>(&CMESSAGE::CSimpleMessage(&message));
	case 2:
		return static_cast<CMESSAGE::CMessage*>(&CMESSAGE::CExtendedMessage(&message));
	case 3:
		return static_cast<CMESSAGE::CMessage*>(&CMESSAGE::CAuthorizedMessage(&message));
	case 4:
		return static_cast<CMESSAGE::CMessage*>(&CMESSAGE::CExtendedAuthorizedMessage(&message));
	case 5:
		return static_cast<CMESSAGE::CMessage*>(&(CMESSAGE::CBigDataMessage(&message)));
	}
}

CAN::messageCAN ProtocolManager::createMessage(CMESSAGE::CMessage* message)
{
	switch (message->protocol)
	{
	case CMESSAGE::CMessage::EProtocol::CInitialProtocol:
		return prepareCANMessageForInitial(message);
	}
}

CAN::messageCAN ProtocolManager::prepareCANMessageForInitial(CMESSAGE::CMessage* message)
{
	CMESSAGE::CInitialMessage* msg = static_cast<CMESSAGE::CInitialMessage*>(message);
	CAN::messageCAN cMsg;
	cMsg.id = std::stoi(msg->toDomain);
	cMsg.data[0] = static_cast<int>(msg->protocol);
	cMsg.data[1] = 100;
	cMsg.data[2] = msg->header;
	for (int i = 3; i < 8; i++)
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