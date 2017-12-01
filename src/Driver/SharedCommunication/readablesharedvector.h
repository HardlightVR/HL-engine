#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/log/trivial.hpp>
#include "SharedTypes.h"
template<typename T> 
class ReadableSharedVector
{

public:
	using ptr_t = boost::interprocess::offset_ptr<void, boost::int32_t, boost::uint64_t>;
	using my_managed_shared_memory = boost::interprocess::basic_managed_shared_memory<
		char,
		boost::interprocess::rbtree_best_fit<boost::interprocess::mutex_family, ptr_t>,
		boost::interprocess::iset_index>;


	using TAlloc = boost::interprocess::allocator<T, my_managed_shared_memory::segment_manager>;
	using TVector = boost::interprocess::vector<T, TAlloc>;
	using MutexGuard = boost::interprocess::scoped_lock<boost::interprocess::named_mutex>;

	ReadableSharedVector(const std::string& memName, const std::string& vecName) :

		m_memName(memName),
		m_vecName(vecName),
		m_segment(),
		m_vector{ nullptr },
		m_mutexName(memName + "-mutex"),
		m_mutex(boost::interprocess::open_only, m_mutexName.c_str())
		
	{
		m_segment = my_managed_shared_memory(boost::interprocess::open_only, m_memName.c_str());
		//if (!m_segment.check_sanity()) {
		//	throw boost::interprocess::interprocess_exception("Failed to open the shared memory for tracking");
		//}
		//Unsure, but I believe checking sanity used to cause access violations. It may still do so. 
		m_vector = m_segment.find<TVector>(m_vecName.c_str()).first;
		if (m_vector == 0) {
			throw boost::interprocess::interprocess_exception("Failed to construct ReadableSharedVector memory");
		}


		assert(0 == strcmp(my_managed_shared_memory::get_instance_name(m_vector), m_vecName.c_str()));
		assert(1 == my_managed_shared_memory::get_instance_length(m_vector));
	
	}

	std::size_t Size() const {
		MutexGuard guard(m_mutex);
		assert(m_vector != nullptr);
		return m_vector->size();
	}

	//requires: Size() > index
	T Get(std::size_t index) const {
		MutexGuard guard(m_mutex);
		assert(m_vector != nullptr);
		return (*m_vector)[static_cast<TVector::size_type>(index)];
	}
	

	std::vector<T> ToVector() const {
		MutexGuard guard(m_mutex);
		assert(m_vector != nullptr);
		std::vector<T> values;
		values.reserve(m_vector->size());
		for (auto it = m_vector->cbegin(); it != m_vector->cend(); ++it) {
			values.push_back(*it);
		}
		return values;
	}


	boost::optional<T> Get(std::function<bool(const T& item)> predicate) const {
		MutexGuard guard(m_mutex);
		assert(m_vector != nullptr);
		const auto it = std::find_if(m_vector->cbegin(), m_vector->cend(), predicate);
		if (it != m_vector->cend()) {
			return *it;
		}
		
		return boost::none;
	
	}



private:
	std::string m_memName;
	std::string m_vecName;
	std::string m_mutexName;
	my_managed_shared_memory m_segment;
	TVector* m_vector;
	mutable boost::interprocess::named_mutex m_mutex;

	

};

