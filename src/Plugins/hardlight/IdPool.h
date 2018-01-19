#pragma once

#include <vector>
#include <mutex>

//This class's job is to hand out ids from a pool.
//The API is thread-safe.
class IdPool {
public:
	IdPool();
	std::size_t Request();

	//You must only release ids that were returned from a call to Request().
	void Release(std::size_t);
private:
	std::size_t m_upperBound;
	std::vector<std::size_t> m_pool;
	std::mutex m_lock;
};