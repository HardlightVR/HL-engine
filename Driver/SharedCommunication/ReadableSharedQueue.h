#pragma once


#include <boost\interprocess\ipc\message_queue.hpp>
#include "shared_data.h"

 

class ReadableSharedQueue
{
public:
	typedef boost::interprocess::message_queue_t<boost::interprocess::offset_ptr<void, boost::int32_t, boost::uint64_t>> my_message_queue;

	ReadableSharedQueue(std::string name):
		m_queue(boost::interprocess::open_only, name.c_str()) 
	{}

	~ReadableSharedQueue() {

	}

	std::vector<uint8_t> Pop() {
		std::vector<uint8_t> data(m_queue.get_max_msg_size());
		my_message_queue::size_type actualLen = 0;
		unsigned int priority = 0;
		
		m_queue.try_receive(&data[0], m_queue.get_max_msg_size(), actualLen, priority);
		data.resize(actualLen);
		assert(actualLen <= data.size());
		return data;
	}
private:
	std::string m_name;
	my_message_queue m_queue;

};

