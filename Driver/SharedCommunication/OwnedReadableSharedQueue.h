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
	{
	
		auto size = m_queue.get_num_msg();
		std::vector<char> tempBuffer;
		tempBuffer.resize(m_queue.get_max_msg_size());
		unsigned int x = 0;
		unsigned int y = 0;
		for (int i = 0; i < size; ++i) {
			if (!m_queue.try_receive(tempBuffer.data(), tempBuffer.size(), x, y)) {
				break;
			}
		}
		
	}

	~OwnedReadableSharedQueue() {
		boost::interprocess::message_queue::remove(m_name.c_str());
	}
	
	std::size_t GetNumMessageAvailable() {
		return m_queue.get_num_msg();
	}
	boost::optional<std::vector<uint8_t>> Pop() {
		
		std::vector<uint8_t> otherData;
		otherData.resize(m_queue.get_max_msg_size());
		unsigned int actualLen = 0;
		unsigned int priority = 0;
		try {
			if (m_queue.try_receive(otherData.data(), m_queue.get_max_msg_size(), actualLen, priority)) {
				otherData.resize(actualLen);
				return otherData;
			}
		}
		catch (const boost::interprocess::interprocess_exception& e) {
			std::cout << e.what();
		}
		//assert(actualLen <= data.size());
		return boost::optional<std::vector<uint8_t>>();
	}

	
private:
	std::string m_name;
	std::size_t m_maxElementSizeBytes;
	boost::interprocess::message_queue m_queue;
};

