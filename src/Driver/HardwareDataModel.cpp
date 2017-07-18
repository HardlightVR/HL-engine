#include "stdafx.h"
#include "HardwareDataModel.h"
#include "HardwareCoordinator.h"
#include "pevent.h"


template<typename T, typename ...Args> 
void notify(const T& container, Args&&...args) {
	for (const auto& cb : container) {
		cb(std::forward<Args>(args)...);
	}
}
HardwareDataModel::HardwareDataModel(HardwareCoordinator & parentCoordinator) : m_parent(parentCoordinator),
m_trackingData(),
m_trackingSubscribers()
{
}

//called every 1ms
void measure() {
	if (total_elapsed < 16) {
		sample(querystate);
	}
	else {
		submit_sampling_frame(querystate->id, this)
	}
}

void HardwareDataModel::OnTrackingUpdate(TrackingCallback callback)
{
	m_trackingSubscribers.push_back(std::move(callback));
}

void HardwareDataModel::OnDeviceConnect(ConnectionCallback cb)
{
	m_onConnectSubscribers.push_back(std::move(cb));
}

void HardwareDataModel::OnDeviceDisconnect(ConnectionCallback cb)
{
	m_onDisconnectSubscribers.push_back(std::move(cb));
}

void HardwareDataModel::Update(const std::string & region, NSVR_Core_Quaternion quat)
{
	m_trackingData[region] = quat;
	notify(m_trackingSubscribers, region, quat);
}

void HardwareDataModel::SetDeviceConnected()
{
	m_connected = true;
	notify(m_onConnectSubscribers);
	
}

void HardwareDataModel::SetDeviceDisconnected()
{
	m_connected = false;
	notify(m_onDisconnectSubscribers);
}

void HardwareDataModel::Raise(const nsvr::pevents::device_event& event)
{
	switch (event.type) {
	case nsvr_device_event_device_connected:
		//static_cast<const nsvr::pevents::device_connected*>(&event);
		break;
	case nsvr_device_event_device_disconnected:
		break;
	case nsvr_device_event_tracking_update:
		NSVR_Core_Quaternion q = static_cast<const nsvr::pevents::tracking_update*>(&event)->quat;
		std::cout << "Got a tracking update: " << q.w << ", " << q.x << "\n";
		
		break;
	default:
		break;
	}
}

HardwareCoordinator & HardwareDataModel::GetParentCoordinator()
{
	return m_parent;
}

void HardwareDataModel::beginMeasuring(nsvr_querystate * querystate)
{
	querystate->
}
