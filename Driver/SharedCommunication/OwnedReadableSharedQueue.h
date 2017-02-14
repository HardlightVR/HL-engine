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
		//std::vector<char> myData;
		//myData.reserve(m_queue.get_max_msg_size());
		std::vector<uint8_t> otherData;
		otherData.resize(m_queue.get_max_msg_size());
	//	assert(otherData.data() != nullptr);
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
		return std::vector<uint8_t>();
	}

	
private:
	std::string m_name;
	std::size_t m_maxElementSizeBytes;
	boost::interprocess::message_queue m_queue;
};

