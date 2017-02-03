#pragma once
#include <boost/interprocess/ipc/message_queue.hpp>

using namespace boost::interprocess;
class ClientMessenger
{
public:
	ClientMessenger();
	~ClientMessenger();
	void Send(void * data, std::size_t size);
private:
	message_queue _msgs;
};

