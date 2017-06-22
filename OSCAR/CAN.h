#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <sstream>
#include <boost\thread.hpp>
class CAN
{
public:
	typedef struct {
		unsigned int id;
		struct
		{
			char rtr : 1;
			char length : 4;
		}header;
		uint8_t data[8];
	}messageCAN;
	std::string buffer_;
	std::bitset<8> domain;
	CAN(int ownId);
	~CAN();
	void begin(uint16_t speed) { this->speed = speed; }
	void setMsgAvailable() { this->msgAvailable = true; }
	void resetMsgAvailable() { this->msgAvailable = false; }
	messageCAN messageRx;
	messageCAN messageTx;
	void sendMessage();
	bool messageAvailable();
	int getId() { return ownId_; }
	void receiveMessage();
	std::string convertToHexString();
private:
	int ownId_;
	uint16_t speed;
	bool msgAvailable;
	void clear();
};

