#include "stdafx.h"
#include "CAN.h"


CAN::CAN(int ownId) : ownId_(ownId), domain(ownId)
{
}


CAN::~CAN()
{
}

void CAN::sendMessage()
{
	std::cout << "_____________________________SENDING MESSAGE____________________________________" << std::endl;
	std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::in);
	std::string isEmpty;
	can_recv >> isEmpty;
	can_recv.close();
	std::cout << "registerMessage::precondition: register is: " << isEmpty << " | " << isEmpty.size() << std::endl;
	std::cout << "MESSAGE ID: " << messageTx.id << std::endl;
	if (isEmpty.size() == 0)
	{
		std::bitset<8> id(static_cast<int>(messageTx.id));
		std::string msg = id.to_string();

		for (const auto &element : messageTx.data)
		{
			std::bitset<8> bitset(static_cast<int>(element));
			std::cout << "element: " << static_cast<int>(element) << " | " << bitset.to_string() << std::endl;
			msg += bitset.to_string();
		}
		std::cout << "CAN::registerMessage: " << msg << std::endl;
		std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
		can_recv << msg;
		can_recv.close();
	}
	else
	{
		std::cout << "BUFFER IS NOT EMPTY" << std::endl;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
		sendMessage();
	}
	std::cout << "___________________________________________________________________________________" << std::endl;
	
}

bool CAN::messageAvailable()
{
	std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::in);
	can_recv >> buffer_;
	can_recv.close();
	std::cout << "Module::messageAvailable: " << buffer_ << " | " << buffer_.size() << std::endl;
	if (buffer_ != "" && buffer_.size() == 72)
	{
		auto domainBinary = buffer_.substr(0, 8);
		std::cout << domainBinary << " | " << domain.to_string()  << std::endl;
		if (domainBinary == domain.to_string())
			return true;
		else
		{
			buffer_ = "";
			return false;
		}

	}
	buffer_ = "";
	return false;
}

void CAN::receiveMessage()
{
	unsigned int id = 0;
	for (int i = 0; i < 8; i++)
	{
		id += std::stoi(buffer_.substr(i, 1)) * std::pow(2, (7 - i));
		std::cout << "TMP equaling id: " << std::stoi(buffer_.substr(i, 1)) << " * " << std::pow(2, (7 - i)) << " = " << id << std::endl;
	}
	messageRx.id = id;
	for (int i = 1; i <= 8; i++)
	{
		int tmp = 0;
		int powTmp = 7;
		for (int j = i * 8; j < (i + 1) * 8; j++)
		{
			tmp += std::stoi(buffer_.substr(j, 1)) * std::pow(2, powTmp);
			std::cout << "TMP equaling data: " << std::stoi(buffer_.substr(j, 1)) << " * " << std::pow(2, powTmp) << " = " << tmp << std::endl;
			powTmp--;
		}
		messageRx.data[i - 1] = tmp;
		std::cout << "Data[" << i - 1 << "] = " << tmp << std::endl;
	}
	buffer_ = "";
	clear();
}

void CAN::clear()
{
	std::fstream can_recv("D:\\private\\OSCAR\\New_Architecture_OSCAR\\OSCAR\\System\\CAN_recv.txt", std::ios::out);
	can_recv << "";
	can_recv.close();
}

std::string CAN::convertToHexString()
{
	std::string hexaString = "";
	for (int i = 0; i < 8; i++)
	{
		std::cout << "CONVERTION: " << static_cast<int>(messageRx.data[i]) << std::endl;
		std::stringstream ss;
		ss << std::hex << static_cast<int>(messageRx.data[i]);
		std::cout << ss.str() << std::endl;
		hexaString += ss.str();
	}
	return hexaString;
}
