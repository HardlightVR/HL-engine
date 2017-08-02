#pragma once

#include "OwnedWritableSharedVector.h"
#include "SharedTypes.h"
#include <boost/interprocess/containers/string.hpp>
#include <memory>

// I'm not entirely sure why we're not just using boost::interprocess::map, instead of finding key then indexing
// Maybe I just didn't see it?
// Maybe with only 10-20 elements this is faster?

// Also this should probably be stored as tuples instead. We are already paying the price of looking up the key in
// a linear fashion, might as well have some kind of locality?

template<typename KeyType, typename DataType>
class OwnedWritableSharedMap
{

public:
	OwnedWritableSharedMap(std::size_t initialElementCapacity, const std::string& name) :
		m_dataMemName(name),
		m_keyMemName(m_dataMemName + "-keys")
	{
		using namespace NullSpace::SharedMemory;
		//Idea: We could also store pairs of key, value
		using DataVec = OwnedWritableSharedVector<DataType>;

		using KeyVec = OwnedWritableSharedVector<KeyType>;

		//note: we could probably allocate both vecs in one shared mem region?
		DataVec::remove(m_dataMemName.c_str());
		KeyVec::remove(m_keyMemName.c_str());
		
		// unsure what the base capacity actually needs to be for a boost::interprocess::vector, but 1024 seems to work
		const int vectorNecessaryByteSize = 1024;
		const int dataByteSize = vectorNecessaryByteSize + sizeof(DataType)*initialElementCapacity;
		const int keyByteSize = vectorNecessaryByteSize + 128 * initialElementCapacity;

		m_data = std::make_unique<DataVec>(m_dataMemName.c_str(), m_dataMemName + "-structure", dataByteSize);
		m_keys = std::make_unique<KeyVec>(m_keyMemName.c_str(), m_keyMemName +"-structure", keyByteSize);


	}

	bool Contains(KeyType key) {
		/* conversion to bool */
		if (m_keys->Find(key)) {
			return true;
		}
		else {
			return false;
		}
		
	}
	void Insert(KeyType key, DataType data) {
		//We create an implicit mapping between key and value by index into the vectors.
		assert(m_data->size() == m_keys->size());

		try {
			m_data->Push(std::move(data));
			m_keys->Push(key);
		}
		catch (const boost::interprocess::interprocess_exception& ex) {
			BOOST_LOG_TRIVIAL(error) << "[IPC::OwnedWritableSharedMap] Error! " << ex.what();
		}
	}

	void Update(KeyType key, DataType data) {
		if (auto foundIndex = m_keys->Find(key)) {
			std::size_t index = *foundIndex;
			m_data->Update(index, std::move(data));
		}
	}

private:
	std::string m_dataMemName;
	std::string m_keyMemName;

	std::unique_ptr<OwnedWritableSharedVector<DataType>> m_data;
	std::unique_ptr<OwnedWritableSharedVector<KeyType>> m_keys;

};


#pragma once
