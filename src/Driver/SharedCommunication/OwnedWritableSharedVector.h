#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/log/trivial.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "SharedTypes.h"
template<typename T> 
class OwnedWritableSharedVector
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

	OwnedWritableSharedVector(const std::string& memName, const std::string& vecName, const std::size_t byteSize) :

		m_memName(memName),
		m_vecName(vecName),
		m_segment(boost::interprocess::create_only, memName.c_str(), byteSize, 0, []() {
			boost::interprocess::permissions perm;
			perm.set_unrestricted();
			return perm;
		}()),
		m_alloc(m_segment.get_segment_manager()),
		m_mutexName(memName + "-mutex"),
		m_mutex(boost::interprocess::open_or_create, m_mutexName.c_str(), []() {
			boost::interprocess::permissions perm;
			perm.set_unrestricted();
			return perm;
		}())
	{
		
		m_vector = m_segment.construct<TVector>(m_vecName.c_str())(m_alloc);
		assert(0 == strcmp(my_managed_shared_memory::get_instance_name(m_vector), m_vecName.c_str()));
		assert(1 == my_managed_shared_memory::get_instance_length(m_vector));
		assert(m_segment.check_sanity());

		if (m_vector == 0) {
			BOOST_LOG_TRIVIAL(error) << "[SharedMem] Unable to construct tracking memory!";
		}

	
	}

	void Push(const T& item) {
		assert(m_vector != nullptr);
		MutexGuard guard(m_mutex);

		
		m_vector->push_back(item);
		

	}

	T Get(std::size_t index) const {
		assert(m_vector != nullptr);

		MutexGuard guard(m_mutex);
		auto element = m_vector->at(index);
		return element;
	}
	void Remove(std::function<bool(const T& item)> predicate) {
		assert(m_vector != nullptr);

		MutexGuard guard(m_mutex);
		m_vector->erase(std::remove_if(m_vector->begin(), m_vector->end(), predicate), m_vector->end());
	}

	std::size_t size() const {
		assert(m_vector != nullptr);

		MutexGuard guard(m_mutex);

		return m_vector->size();
	}

	void Update(std::size_t index, const T& item) {
		assert(m_vector != nullptr);

		MutexGuard guard(m_mutex);

		if (m_vector != nullptr) {
			(*m_vector)[index] = item;
			
			//std::cout << "Begin shared mem quats\n";
			//for (const auto& a: *m_vector) {
			//	std::cout << a.x << ", " << a.y << ", " << a.z << ", " << a.w << '\n';
			//}
			//std::cout << "End shared mem quats\n";

		}
	}

	boost::optional<std::size_t> Find(std::function<bool(const T& item)> predicate)  const{
		assert(m_vector != nullptr);

		MutexGuard guard(m_mutex);

		const auto it = std::find_if(m_vector->cbegin(), m_vector->cend(), predicate);
		if (it != m_vector->cend()) {
			return it - m_vector->cbegin();
		}
		else {
			return boost::none;
		}
	}
	boost::optional<std::size_t> Find(const T& item) const{
		assert(m_vector != nullptr);

		MutexGuard guard(m_mutex);

		const auto it = std::find(m_vector->cbegin(), m_vector->cend(), item);
		if (it != m_vector->cend()) {
			return it - m_vector->cbegin();
		}
		else {
			return boost::none;
		}
		
	}

	void Mutate(std::size_t index, std::function<void(T&)> mut) {
		assert(m_vector != nullptr);

		MutexGuard guard(m_mutex);

		mut(m_vector->at(index));
	}

	~OwnedWritableSharedVector() {
	//	m_segment.destroy<TVector>(m_vecName.c_str());
	}


	static bool remove(const char* name) {
		std::string mutex(name);
		mutex += "-mutex";
		bool removed_mem = boost::interprocess::shared_memory_object::remove(name);
	//	bool removed_mut = boost::interprocess::named_mutex::remove(mutex.c_str());

		return removed_mem ;
	}

private:
	std::string m_memName;
	std::string m_vecName;
	std::string m_mutexName;
	my_managed_shared_memory m_segment;
	TAlloc m_alloc;
	TVector* m_vector;
	mutable boost::interprocess::named_mutex m_mutex;
	

};

