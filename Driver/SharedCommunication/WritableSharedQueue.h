#pragma once


#include <boost\interprocess\ipc\message_queue.hpp>
#include "shared_data.h"

 

class WritableSharedQueue
{

public:
	WritableSharedQueue(std::string name) :

		m_name(name),
		m_queue(boost::interprocess::open_only, name.c_str()),
		m_maxElementSizeBytes(256)
	{}

	~WritableSharedQueue() {
	}

	bool Push(const void* data, std::size_t sizeBytes) {
		assert(sizeBytes <= m_maxElementSizeBytes);
		return m_queue.try_send(data, sizeBytes, 1);
	}
private:
	std::string m_name;
	std::size_t m_maxElementSizeBytes;
	boost::interprocess::message_queue m_queue;

};

