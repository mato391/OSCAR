#pragma once
#include "CAN.h"
#include <string>

namespace CMESSAGE
{
	class CMessage
	{
	public:
		enum class EProtocol
		{
			CInitialProtocol,
			CSimpleProtocol,
			CExtendedProtocol,
			CAuthorizedProtocol,
			CExtendedAuthorizedProtocol,
			CBigDataProtocol
		};
		EProtocol protocol;
		std::string fromDomain;
		std::string toDomain;
		int header;
		virtual EProtocol getProtocol() = 0;
	};

	class CInitialMessage :
		public CMessage
	{
	public:
		CInitialMessage(CAN::messageCAN* msg)
		{
			protocol = CMessage::EProtocol::CInitialProtocol;
			std::stringstream hexa;
			hexa << std::hex << msg->data[1];
			fromDomain = hexa.str();
			toDomain = msg->id;
			header = msg->data[2];
		}
		EProtocol getProtocol() { return protocol; }
	};

	class CSimpleMessage :
		public CMessage
	{
	public:
		CSimpleMessage(CAN::messageCAN* msg)
		{
			protocol = CMessage::EProtocol::CSimpleProtocol;
			std::stringstream hexa;
			hexa << std::hex << msg->data[1];
			fromDomain = hexa.str();
			toDomain = msg->id;
			header = msg->data[2];
			port = msg->data[3];
			value = msg->data[4];
		}
		int port;
		int value;
		EProtocol getProtocol() { return protocol; }
	};

	class CExtendedMessage :
		public CMessage
	{
	public:
		CExtendedMessage(CAN::messageCAN* msg)
		{
			protocol = CMessage::EProtocol::CExtendedProtocol;
			std::stringstream hexa;
			hexa << std::hex << msg->data[1];
			fromDomain = hexa.str();
			toDomain = msg->id;
			header = msg->data[2];
			port = msg->data[3];
			value = msg->data[4];
			additional = msg->data[5];
		};
		int port;
		int value;
		int additional;
		EProtocol getProtocol() { return protocol; }
	};

	class CAuthorizedMessage :
		public CMessage
	{
	public:
		CAuthorizedMessage(CAN::messageCAN* msg)
		{
			protocol = CMessage::EProtocol::CAuthorizedProtocol;
			std::stringstream hexa;
			hexa << std::hex << msg->data[1];
			fromDomain = hexa.str();
			toDomain = msg->id;
			header = msg->data[2];
			login = msg->data[3];
			value = msg->data[4];
			additional = msg->data[5];
		};
		int login;
		int value;
		int additional;
		EProtocol getProtocol() { return protocol; }
	};

	class CExtendedAuthorizedMessage :
		public CMessage
	{
	public:
		CExtendedAuthorizedMessage(CAN::messageCAN* msg)
		{
			protocol = CMessage::EProtocol::CExtendedAuthorizedProtocol;
			std::stringstream hexa;
			hexa << std::hex << msg->data[1];
			fromDomain = hexa.str();
			toDomain = msg->id;
			header = msg->data[2];
			login = msg->data[3];
			pass = msg->data[4];
			value = msg->data[5];
			additional = msg->data[6];
		};
		int login;
		int pass;
		int value;
		int additional;
		EProtocol getProtocol() { return protocol; }
	};

	class CBigDataMessage :
		public CMessage
	{
	public:
		CBigDataMessage(CAN::messageCAN* msg)
		{
			protocol = CMessage::EProtocol::CBigDataProtocol;
			std::stringstream hexa;
			hexa << std::hex << msg->data[1];
			fromDomain = hexa.str();
			toDomain = msg->id;
			header = msg->data[2];
			wordSize = msg->data[3];
			for (int i = 3; i < 8; i++)
				data.push_back(msg->data[i]);
		};
		int wordSize;
		std::vector<int> data;
		EProtocol getProtocol() { return protocol; }
	};


}
