#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "ChatMessage.hpp"

using boost::asio::ip::tcp;



typedef std::deque<ChatMessage> chat_message_queue;

class EngineCommunicationService
{
public:
	EngineCommunicationService();
	~EngineCommunicationService();


};

