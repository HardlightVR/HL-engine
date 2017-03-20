#pragma once

#include <boost\interprocess\shared_memory_object.hpp>
#include <boost\interprocess\mapped_region.hpp>
#include "shared_data.h"

 
template<typename T>
class WritableSharedObject
{

public:

	WritableSharedObject(std::string name) : m_name(name){
		using namespace boost::interprocess;
		permissions perms;
		perms.set_unrestricted();
		m_object = shared_memory_object(open_or_create, m_name.c_str(), read_write, perms);
		m_object.truncate(sizeof(shared_data<T>));
		m_region = mapped_region(m_object, read_write);
		m_data = new (m_region.get_address()) shared_data<T>;

	}

	~WritableSharedObject() {
		boost::interprocess::shared_memory_object::remove(m_name.c_str());
	}

	void Write(T data) {
		m_data->SetData(data);
	}
private:
	std::string m_name;
	boost::interprocess::shared_memory_object m_object;
	boost::interprocess::mapped_region m_region;
	shared_data<T>* m_data;
};

