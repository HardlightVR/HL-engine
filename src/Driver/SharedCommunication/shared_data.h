#pragma once
#include <boost\interprocess\sync\sharable_lock.hpp>
#include <boost\interprocess\sync\named_sharable_mutex.hpp>
#include <boost\interprocess\sync\interprocess_upgradable_mutex.hpp>
template<typename T>
struct shared_data {
private:
	typedef boost::interprocess::interprocess_upgradable_mutex upgradable_mutex_type;
	mutable upgradable_mutex_type m_mutex;
	T m_data;
public:
	shared_data() {}

	T GetData() const {
		boost::interprocess::sharable_lock<upgradable_mutex_type> lock(m_mutex);
		return m_data;
	}

	void SetData(T data) {
		boost::interprocess::scoped_lock<upgradable_mutex_type> lock(m_mutex);
		m_data = data;
	}
};