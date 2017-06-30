#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/log/trivial.hpp>
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

	OwnedWritableSharedVector(const std::string& memName, const std::string& vecName, const std::size_t size) :

		m_memName(memName),
		m_vecName(vecName),
		m_segment(boost::interprocess::create_only, memName.c_str(), size, 0, []() {
			boost::interprocess::permissions perm;
			perm.set_unrestricted();
			return perm;
		}()),
		m_alloc(m_segment.get_segment_manager())
	{
		
		m_vector = m_segment.construct<TVector>(m_vecName.c_str())(m_alloc);
		assert(0 == strcmp(my_managed_shared_memory::get_instance_name(m_vector), m_vecName.c_str()));
		assert(1 == my_managed_shared_memory::get_instance_length(m_vector));
		assert(m_segment.check_sanity());

		if (m_vector == 0) {
			BOOST_LOG_TRIVIAL(error) << "[SharedMem] Unable to construct tracking memory!";
		}
	}

	void Push(T item) {
		if (m_vector != nullptr) {
			m_vector->push_back(std::move(item));
		}
	}

	std::size_t size() const {
		return m_vector->size();
	}

	void Update(std::size_t index, T item) noexcept {
		if (m_vector != nullptr) {
			(*m_vector)[index] = item;
			
			std::cout << "Begin shared mem quats\n";
			for (const auto& a: *m_vector) {
				std::cout << a.x << ", " << a.y << ", " << a.z << ", " << a.w << '\n';
			}
			std::cout << "End shared mem quats\n";

		}
	}

	boost::optional<std::size_t> Find(const T& item) const{
		int pos = std::find(m_vector->cbegin(), m_vector->cend(), item) - m_vector->cbegin();
		return pos;
	}

	~OwnedWritableSharedVector() {
		m_segment.destroy<TVector>(m_vecName.c_str());
	}


	static bool remove(const char* name) {
		return boost::interprocess::shared_memory_object::remove(name);
	}

private:
	std::string m_memName;
	std::string m_vecName;
	my_managed_shared_memory m_segment;
	TAlloc m_alloc;
	TVector* m_vector;
	

};

