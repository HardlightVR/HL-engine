#include "stdafx.h"
#include "Messenger.h"


Messenger::Messenger():
	_context(1), 
	_broadcastSocket(_context, ZMQ_PUB), 
	_pollSocket(_context, ZMQ_SUB)
{

	//The ZMQ sockets require some tweaking. For haptic_requests, we want the "high water mark" to be fairly small. Meaning,
	//if we receive more messages than the HWM, we discard them. This is because we don't want old haptics. 
	//Alternatively, we could use ZMQ_CONFLATE, which only keeps one message, but if we don't process it fast enough it's gone. 
	//The parameter for RCVHWM may arbitrarily map to an amount of messages, not sure. See ZMQ docs. 
	//Also, it must be set before binding (see docs)
	_pollSocket.setsockopt(ZMQ_RCVHWM, 16);
	_pollSocket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

	_pollSocket.bind("tcp://127.0.0.1:9452");
	//Since it's a sub socket, we need a topic to subscribe to. Since we don't use multiple topics, we use "".

	//We don't want server updates buffered at all. Might get stale IMU data, disconnections, reconnections, etc. Also,
	//say the application froze - we don't want them to receive a bunch of junk data, just the most recent.
	int confl = 1;

	_broadcastSocket.bind("tcp://127.0.0.1:9453");
	_broadcastSocket.setsockopt(ZMQ_CONFLATE, &confl, sizeof(confl));
}


Messenger::~Messenger()
{
	_broadcastSocket.close();
	_pollSocket.close();
	
	_context.close();
}

bool Messenger::Broadcast(void * data, std::size_t length)
{
	return true;
}

bool Messenger::Receive(const std::function<void(void*data, std::size_t length)>& processData)
{
	zmq::message_t msg;
	if (_pollSocket.recv(&msg)) {
		processData(msg.data(), msg.size());
		return true;
	}
	return false;
}

bool Messenger::Poll(const std::function<void(void*data, std::size_t length)>& processData)
{
	zmq::message_t msg;
	if (_pollSocket.recv(&msg, ZMQ_DONTWAIT)) {
		//We have ownership of the message!
		processData(msg.data(), msg.size());
		return true;
	}
//	std::cout << "Nothing received\n";
	return false;
}
