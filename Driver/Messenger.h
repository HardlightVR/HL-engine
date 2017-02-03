#pragma once
#include <boost/interprocess/ipc/message_queue.hpp>

struct ExecutionCommand {
	int Location;
	int Effect;
	short Command;
};


using namespace boost::interprocess;
class Messenger
{
public:
	Messenger();
	~Messenger();
	bool Broadcast(void* data, std::size_t length);
	void Receive(const std::function<void(void const* data, std::size_t length)>);
	bool Poll(const std::function<void(void const * data, std::size_t length)>&);
	void Disconnect();
private:
	std::function<void(void const* data, std::size_t length)> _process;
	static const int _maxEffectArraySize = sizeof(ExecutionCommand) * 16;
	uint8_t _data[_maxEffectArraySize];
	std::thread _worker;
	message_queue _queue;

	std::atomic<bool> _running;

	void doWork();
		
};


