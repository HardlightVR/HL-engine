#include "stdafx.h"
#include "HardwareDataModel.h"
#include "HardwareCoordinator.h"

template<typename T, typename ...Args> 
void notify(const T& container, Args&&...args) {
	for (const auto& cb : container) {
		cb(std::forward<Args>(args)...);
	}
}
HardwareDataModel::HardwareDataModel(HardwareCoordinator & parentCoordinator) : m_parent(parentCoordinator)
{
}

HardwareDataModel::HardwareDataModel()
{

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

void HardwareDataModel::Raise(const nsvr::pevents::pevent& event)
{
	std::cout << "Received event of type " << event.type << "!\n";
}

HardwareCoordinator & HardwareDataModel::GetParentCoordinator()
{
	return m_parent;
}
