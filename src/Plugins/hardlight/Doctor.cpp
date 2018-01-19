#include "stdafx.h"
#include "Doctor.h"
#include <numeric>

Doctor::Status Doctor::get_device_status(nsvr_device_id id) const
{
	std::lock_guard<std::mutex> guard(m_lock);

	auto it = m_devices.find(id);
	if (it != m_devices.end()) {
		return it->second;
	}

	return Status::Unknown;
}

std::vector<nsvr_device_id> Doctor::get_devices() const
{
	std::lock_guard<std::mutex> guard(m_lock);

	std::vector<nsvr_device_id> devices;
	for (const auto& kvp : m_devices) {
		devices.push_back(kvp.first);
	}
	return devices;
}
//
//std::vector<nsvr_device_id> Doctor::get_potential_devices() const
//{
//	std::vector<nsvr_device_id> devices;
//	for (const auto& kvp : m_devices) {
//		if (kvp.second == Status::CheckingDiagnostics) {
//			devices.push_back(kvp.first);
//		}
//	}
//	return devices;
//}
//
//std::vector<nsvr_device_id> Doctor::get_verified_devices() const
//{
//	std::vector<nsvr_device_id> devices;
//	for (const auto& kvp : m_devices) {
//		if (kvp.second == Status::OkDiagnostics) {
//			devices.push_back(kvp.first);
//		}
//	}
//	return devices;
//}

void Doctor::notify_device_status(nsvr_device_id id, Status status)
{
	std::lock_guard<std::mutex> guard(m_lock);

	m_devices[id] = status;
}
