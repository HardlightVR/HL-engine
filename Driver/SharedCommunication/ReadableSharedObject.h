#pragma once

#include <boost\interprocess\shared_memory_object.hpp>
#include <boost\interprocess\mapped_region.hpp>
#include "shared_data.h"


template<typename T>
class ReadableSharedObject
{

public:

	ReadableSharedObject(std::string name){
		using namespace boost::interprocess;
		m_object = shared_memory_object(open_only, name.c_str(), read_write);
		m_region = mapped_region(m_object, read_write);
		m_data = static_cast<shared_data<T>*>(m_region.get_address());
	}
	~ReadableSharedObject() {}

	T Read() {
		return m_data->GetData();
	}
private:

	boost::interprocess::shared_memory_object m_object;
	boost::interprocess::mapped_region m_region;
	shared_data<T>* m_data;
};

