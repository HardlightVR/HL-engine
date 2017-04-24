#pragma once


#include <boost\interprocess\ipc\message_queue.hpp>
#include "shared_data.h"

 

class WritableSharedQueue
{

public:
	typedef boost::interprocess::message_queue_t<boost::interprocess::offset_ptr<void, boost::int32_t, boost::uint64_t>> my_message_queue;

	WritableSharedQueue(std::string name) :

		m_name(name),
		m_queue(boost::interprocess::open_only, name.c_str()),
		m_maxElementSizeBytes(256)
	{}

	~WritableSharedQueue() {
	}

	bool Push(const void* data, std::size_t sizeBytes) {
		assert(sizeBytes <= m_maxElementSizeBytes);
		return m_queue.try_send(data, (unsigned int) sizeBytes, 1);
	}
private:
	std::string m_name;
	std::size_t m_maxElementSizeBytes;
	my_message_queue m_queue;

};

