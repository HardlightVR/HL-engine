#pragma once
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>


template<typename KeyType, typename MappedType>
class OwnedWritableSharedMap
{

public:
	using MutexGuard = boost::interprocess::scoped_lock<boost::interprocess::named_mutex>;

	OwnedWritableSharedMap(const std::string& name) :
		m_dataMemName(name),
		m_segment(boost::interprocess::create_only, (name + "-data").c_str(), 65536, 0, []() {
			boost::interprocess::permissions perm;
			perm.set_unrestricted();
			return perm;
		}()),
		m_allocInst(m_segment.get_segment_manager()),
		m_mutexName(name + "-mutex"),
		m_mutex(boost::interprocess::open_or_create, m_mutexName.c_str())
	{
	
		m_map = m_segment.construct<Map>(m_dataMemName.c_str())(std::less<KeyType>(), m_allocInst);
		assert(m_segment.check_sanity());
		

	}

	bool Contains(KeyType key) {
		MutexGuard guard(m_mutex);
		return m_map->find(key) != m_map->end();
	}

	

	void Update(KeyType key, const MappedType& data) {
		MutexGuard guard(m_mutex);

		m_map->operator[](key) = data;
		
	}

	~OwnedWritableSharedMap() {
		//m_segment.destroy<Map>(m_dataMemName.c_str());

	}
	static bool remove(const char* name) {
		bool ok = boost::interprocess::named_mutex::remove((name + std::string("-mutex")).c_str());
		return ok && boost::interprocess::shared_memory_object::remove((name + std::string("-data")).c_str());
	}

private:

	using ptr_t = boost::interprocess::offset_ptr<void, boost::int32_t, boost::uint64_t>;
	using my_managed_shared_memory = boost::interprocess::basic_managed_shared_memory<
		char,
		boost::interprocess::rbtree_best_fit<boost::interprocess::mutex_family, ptr_t, 8>,
		boost::interprocess::iset_index>;

	using ValueType = std::pair<const KeyType, MappedType>;

	std::string m_dataMemName;
	std::string m_mutexName;
	using ShmemAllocator = boost::interprocess::allocator<ValueType, my_managed_shared_memory::segment_manager>;

	using Map = boost::interprocess::map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator>;
	my_managed_shared_memory m_segment;
	ShmemAllocator m_allocInst;
	Map* m_map;
	boost::interprocess::named_mutex m_mutex;
};


#pragma once
