#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/log/trivial.hpp>
#include "SharedTypes.h"
template<typename T> 
class ReadableSharedVector
{

public:
	
	using TVector = boost::interprocess::vector<T>;

	ReadableSharedVector(const std::string& memName, const std::string& vecName) :

		m_memName(memName),
		m_vecName(vecName),
		m_segment()
		
	{
		m_segment = boost::interprocess::managed_shared_memory(boost::interprocess::open_only, m_memName.c_str());

		m_vector = m_segment.find<TVector>(m_vecName.c_str()).first;
		if (m_vector == 0) {
			BOOST_LOG_TRIVIAL(error) << "[SharedMem] Unable to construct tracking memory!";
		}
	}

	std::size_t size() const {
		return m_vector->size();
	}

	T Get(std::size_t index) const {
		return m_vector->at(index);
	}

	boost::optional<std::size_t> Find(const T& item) const {
		int pos = std::find(m_vector->cbegin(), m_vector->cend(), item) - m_vector->cbegin();
		if (pos < m_vector->size()) {
			return pos;
		}
		else {
			return boost::none;
		}
	}

	~ReadableSharedVector() {
		
	}


private:
	std::string m_memName;
	std::string m_vecName;
	boost::interprocess::managed_shared_memory m_segment;
	TVector* m_vector;
	

};

