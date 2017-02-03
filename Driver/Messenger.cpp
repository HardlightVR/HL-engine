#include "stdafx.h"
#include "Messenger.h"


Messenger::Messenger():
	_queue(open_or_create, "ns_haptics_queue", 100, _maxEffectArraySize), _running{true},
	_process([](void const*, std::size_t) {})

{
	_worker = std::thread(boost::bind(&Messenger::doWork, this));
	
}


Messenger::~Messenger()
{
	if (_worker.joinable()) {
		_worker.join();
	}
	message_queue::remove("ns_haptics_queue");


}

bool Messenger::Broadcast(void * data, std::size_t length)
{
	return true;
}

void Messenger::Receive(const std::function<void(void const* data, std::size_t length)> processData)
{
	_process = processData;
}

bool Messenger::Poll(const std::function<void(void const *data, std::size_t length)>& processData)
{
	return true;
}

void Messenger::Disconnect()
{
	_running.store(false);
}

void Messenger::doWork()
{
	while (_running.load()) {
		std::fill(_data, _data + _maxEffectArraySize, 0);
		unsigned int actualLen = 0;
		unsigned int prio = 0;
		try {
			_queue.try_receive(&_data, _maxEffectArraySize, actualLen, prio);
		}
		catch (boost::interprocess::interprocess_exception& ec) {
			std::cout << ec.what();
		}
		if (actualLen > 0) {

			_process(_data, actualLen);
		}
		
	}
}
