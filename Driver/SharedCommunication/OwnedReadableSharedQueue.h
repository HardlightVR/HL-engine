#pragma once


#include <boost\interprocess\ipc\message_queue.hpp>
#include "shared_data.h"

 

class OwnedReadableSharedQueue
{
public:
	OwnedReadableSharedQueue(std::string name, std::size_t maxElements, std::size_t maxElementSizeBytes) :
		m_maxElementSizeBytes(maxElementSizeBytes),
		m_name(name),
		m_queue(boost::interprocess::open_or_create, m_name.c_str(), maxElements, maxElementSizeBytes)
	{}

	~OwnedReadableSharedQueue() {
		boost::interprocess::message_queue::remove(m_name.c_str());
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
	std::size_t m_maxElementSizeBytes;
	boost::interprocess::message_queue m_queue;
};

