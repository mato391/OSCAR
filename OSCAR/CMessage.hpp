#pragma once
#include "CAN.h"
#include <string>
#include <iostream>

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
			CBigDataProtocol,
			CEmpty
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
		CInitialMessage() {};
		CInitialMessage(CAN::messageCAN* msg)
		{
			std::cout << "CInitialMessage: toDomain " << static_cast<int>(msg->id) << std::endl;
			std::cout << "CInitialMessage: header: " << static_cast<int>(msg->data[2]) << std::endl;
			
			protocol = CMessage::EProtocol::CInitialProtocol;
			std::stringstream hexa;
			hexa << std::hex << static_cast<int>(msg->data[1]);
			std::cout << "CInitialMessage: fromDomain: " << hexa.str() << std::endl;
			fromDomain = (hexa.str().size() == 1) ? "0x0" + hexa.str() : "0x"+hexa.str();
			toDomain = std::to_string(static_cast<int>(msg->id));
			header = static_cast<int>(msg->data[2]);
			optional1 = msg->data[3];
			optional2 = msg->data[4];
		}
		int optional1;
		int optional2;
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
			hexa << std::hex << static_cast<int>(msg->data[1]);
			fromDomain = (hexa.str().size() == 1) ? "0x0" + hexa.str() : "0x" + hexa.str();
			toDomain = std::to_string(static_cast<int>(msg->id));
			header = static_cast<int>(msg->data[2]);
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
		CExtendedMessage() {};
		CExtendedMessage(CAN::messageCAN* msg)
		{
			protocol = CMessage::EProtocol::CExtendedProtocol;
			std::stringstream hexa;
			hexa << std::hex << static_cast<int>(msg->data[1]);
			fromDomain = (hexa.str().size() == 1) ? "0x0" + hexa.str() : "0x" + hexa.str();
			toDomain = std::to_string(static_cast<int>(msg->id));
			header = static_cast<int>(msg->data[2]);
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

	class CEmpty :
		public CMessage
	{
	public:
		CEmpty()
		{
			protocol = CMessage::EProtocol::CEmpty;
		};
		EProtocol getProtocol() { return protocol; }
	};


}
