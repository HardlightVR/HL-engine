#pragma once

#include "ReadableSharedVector.h"
#include "SharedTypes.h"

#include <boost/interprocess/containers/string.hpp>
#include <memory>
class ReadableSharedTracking
{

public:
	ReadableSharedTracking() :
		m_quatMemName("ns-tracking-mem1"),
		m_keyMemName("ns-tracking-mem2")
	{
		//Idea: We could also store pairs of key, value
		using QuatVec = ReadableSharedVector<NullSpace::SharedMemory::Quaternion>;
		using KeyVec = ReadableSharedVector<boost::interprocess::string>;
		
	

		m_quaternions = std::make_unique<QuatVec>(m_quatMemName.c_str(), "ns-tracking-quats");
		m_keys = std::make_unique<KeyVec>(m_keyMemName.c_str(), "ns-tracking-keys");


	}
	


private:
	std::string m_quatMemName;
	std::string m_keyMemName;

	std::unique_ptr<ReadableSharedVector<NullSpace::SharedMemory::Quaternion>> m_quaternions;
	std::unique_ptr<ReadableSharedVector<boost::interprocess::string>> m_keys;

};


