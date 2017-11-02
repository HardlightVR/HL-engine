#include "stdafx.h"
#include "IdPool.h"


#include <numeric>

IdPool::IdPool()
	: m_pool(10, 0)
	, m_upperBound(10)
{
	std::iota(m_pool.begin(), m_pool.end(), 0);
	std::reverse(m_pool.begin(), m_pool.end());

}

std::size_t IdPool::Request()
{
	if (m_pool.empty()) {
		m_pool.push_back(m_upperBound);
		m_upperBound++;
	}

	auto id = m_pool.back();
	m_pool.pop_back();
	return id;
}

void IdPool::Release(std::size_t val)
{
	m_pool.push_back(val);
}
