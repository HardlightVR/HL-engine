#pragma once
#include "zmq.hpp"

class Messenger
{
public:
	Messenger();
	~Messenger();
	bool Broadcast(void* data, std::size_t length);
	bool Receive(const std::function<void(void* data, std::size_t length)>&);
	bool Poll(const std::function<void(void* data, std::size_t length)>&);
private:
	zmq::context_t _context;		//order dependency (1)
	zmq::socket_t _broadcastSocket; //order dependency (2)
	zmq::socket_t _pollSocket;		//order dependency (3)
};


