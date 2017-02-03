#include "stdafx.h"
#include "ClientMessenger.h"


ClientMessenger::ClientMessenger():
	_msgs(open_only, "ns_haptics_queue")
{
}


ClientMessenger::~ClientMessenger()
{
}

void ClientMessenger::Send(void * data, std::size_t size)
{
	bool result = _msgs.try_send(data, size, 1);
	if (!result) {
		std::cout << " failed to send\n";
	}
}
