#pragma once


#include "PluginAPI.h"
class HardwareDataModel {
public:
	using TrackingCallback = std::function<void(const std::string&,  NSVR_Core_Quaternion)>;
	using ConnectionCallback = std::function<void()>;

	void OnTrackingUpdate(TrackingCallback callback);
	void OnDeviceConnect(ConnectionCallback cb);
	void OnDeviceDisconnect(ConnectionCallback cb);

	void Update(const std::string& region, NSVR_Core_Quaternion quat);
	void SetDeviceConnected();
	void SetDeviceDisconnected();
private:
	//unsustainable
	std::vector<TrackingCallback> m_trackingSubscribers;
	std::vector<ConnectionCallback> m_onConnectSubscribers;
	std::vector<ConnectionCallback> m_onDisconnectSubscribers;

	std::unordered_map<std::string, NSVR_Core_Quaternion> m_trackingData;

	bool m_connected;
};