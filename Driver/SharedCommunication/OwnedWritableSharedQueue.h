#pragma once


#include <boost\interprocess\ipc\message_queue.hpp>
#include "shared_data.h"

 

class OwnedWritableSharedQueue
{
public:
	
	typedef boost::interprocess::message_queue_t<boost::interprocess::offset_ptr<void, boost::int32_t, boost::uint64_t>> my_message_queue;
	OwnedWritableSharedQueue(std::string name, std::size_t maxElements, std::size_t maxElementSizeBytes) :
		m_maxElementSizeBytes(maxElementSizeBytes),
		m_name(name),
		m_queue(boost::interprocess::open_or_create, m_name.c_str(), maxElements, maxElementSizeBytes)
	{
		auto size = m_queue.get_num_msg();
		std::vector<char> tempBuffer;
		tempBuffer.resize(m_queue.get_max_msg_size());
		my_message_queue::size_type x = 0;
		unsigned int y = 0;
		for (std::size_t i = 0; i < size; ++i) {
			if (!m_queue.try_receive(tempBuffer.data(), tempBuffer.size(), x, y)) {
				break;
			}
		}
	}

	~OwnedWritableSharedQueue() {
		boost::interprocess::message_queue::remove(m_name.c_str());
	}

	bool Push(const void* data, std::size_t sizeBytes) {
		assert(sizeBytes <= m_maxElementSizeBytes);
		return m_queue.try_send(data, sizeBytes, 1);
	}

	
private:
	std::string m_name;
	std::size_t m_maxElementSizeBytes;
	my_message_queue m_queue;
};

