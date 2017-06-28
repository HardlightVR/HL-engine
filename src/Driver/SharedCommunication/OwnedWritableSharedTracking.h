#pragma once

#include "OwnedWritableSharedVector.h"
#include "SharedTypes.h"

#include <boost/interprocess/containers/string.hpp>
#include <memory>
class OwnedWritableSharedTracking
{

public:
	OwnedWritableSharedTracking() :
		m_quatMemName("ns-tracking-mem1"),
		m_keyMemName("ns-tracking-mem2")
	{
		//Idea: We could also store pairs of key, value
		using QuatVec = OwnedWritableSharedVector<NullSpace::SharedMemory::Quaternion>;
		using KeyVec = OwnedWritableSharedVector<boost::interprocess::string>;
		
		//note: we could probably allocate both vecs in one shared mem region?
		QuatVec::remove(m_quatMemName.c_str());
		KeyVec::remove(m_keyMemName.c_str());


		m_quaternions = std::make_unique<QuatVec>(m_quatMemName.c_str(), "ns-tracking-quats", 2048);
		m_keys = std::make_unique<KeyVec>(m_keyMemName.c_str(), "ns-tracking-keys", 2048);


	}
	

	void Insert(const char* key, NullSpace::SharedMemory::Quaternion quat) {
		//We create an implicit mapping between key and value by index into the vectors.
		assert(m_quaternions->size() == m_keys->size());

		m_quaternions->Push(std::move(quat));
		m_keys->Push(key);
	}

	void Update(const char* key, NullSpace::SharedMemory::Quaternion quat) {
		if (auto foundIndex = m_keys->Find(key)) {
			std::size_t index = *foundIndex;
			m_quaternions->Update(index, std::move(quat));
		}
	}

private:
	std::string m_quatMemName;
	std::string m_keyMemName;

	std::unique_ptr<OwnedWritableSharedVector<NullSpace::SharedMemory::Quaternion>> m_quaternions;
	std::unique_ptr<OwnedWritableSharedVector<boost::interprocess::string>> m_keys;

};


