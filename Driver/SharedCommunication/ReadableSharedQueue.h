#pragma once


#include <boost\interprocess\ipc\message_queue.hpp>
#include "shared_data.h"

 

class ReadableSharedQueue
{
public:
	ReadableSharedQueue(std::string name):
		m_queue(boost::interprocess::open_only, name.c_str()) 
	{}

	~ReadableSharedQueue() {

	}

	std::vector<uint8_t> Pop() {
		std::vector<uint8_t> data;
		data.reserve(m_queue.get_max_msg_size());
		unsigned int actualLen = 0;
		unsigned int priority = 0;
		m_queue.try_receive(data.data(), data.size(), actualLen, priority);
		assert(actualLen <= data.size());
		return data;
	}
private:
	std::string m_name;
	boost::interprocess::message_queue m_queue;

};

