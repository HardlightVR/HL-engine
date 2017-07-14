#include "stdafx.h"
#include "HardlightPlugin.h"
#include "PluginAPI.h"
#include <iostream>
#include <typeinfo>
HardlightPlugin::HardlightPlugin() :
	m_io(std::make_shared<IoService>()),
	m_dispatcher(),
	m_adapter(std::make_unique<BoostSerialAdapter>(m_io->GetIOService())),
	m_firmware(m_adapter, m_io->GetIOService()),
	m_monitor(std::make_shared<KeepaliveMonitor>(m_io->GetIOService(), m_firmware)),
	m_synchronizer(std::make_unique<Synchronizer>(m_adapter->GetDataStream(), m_dispatcher, m_io->GetIOService())),
	m_device(),
	m_eventPull(m_io->GetIOService(), boost::posix_time::milliseconds(5)),
	m_imus(m_dispatcher),
	m_mockTracking(m_io->GetIOService(), boost::posix_time::millisec(16)),
	m_coreApi()

{
	
	m_adapter->SetMonitor(m_monitor);

	m_monitor->SetDisconnectHandler([&]() {
	//	m_coreApi["status"].call<nsvr_core_status_cb>(false);
	});

	m_monitor->SetReconnectHandler([&]() {
		//m_coreApi["status"].call<nsvr_core_status_cb>(true);
	});

	m_adapter->Connect();
	
	m_synchronizer->BeginSync();


	m_eventPull.SetEvent([&]() {
		constexpr auto ms_fraction_of_second = (1.0f / 1000.f);
		auto dt = 5 * ms_fraction_of_second;

	
		auto commands = m_device.GenerateHardwareCommands(dt);
		m_firmware.Execute(commands);

	
		
	});

	m_eventPull.Start();

	m_imus.OnTracking([&](const std::string& id, NSVR_Core_Quaternion quat) {
		//m_coreApi.at("tracking").call<nsvr_core_tracking_cb>(id.c_str(), &quat);
	});

	
	m_imus.AssignMapping(0x12, Imu::Chest, "chest");

	
	m_mockTracking.SetEvent([&]() {
		static float begin = 0;
		
		packet fakeTrackingData;
		memset(&fakeTrackingData.raw, 0, 16);

		float ms = begin;

		fakeTrackingData.raw[2] = 0x99;
		fakeTrackingData.raw[11] = 0x12;

		memcpy(&fakeTrackingData.raw[3], &ms, sizeof(ms));
	//	fakeTrackingData.raw[7] = s.count() & 0x0F;
		m_dispatcher.Dispatch(fakeTrackingData);
		begin += 0.05f;
		if (begin >= 1.0) { begin = -1.0f; }
	
	});

	m_mockTracking.Start();
}

HardlightPlugin::~HardlightPlugin()
{
	m_eventPull.Stop();
	m_mockTracking.Stop();
	m_io->Shutdown();

}






template<typename T, typename...Args>
void getCallback(HardlightPlugin::core_callbacks* callbacks, const NSVR_Configuration* config, const std::string& cbName) {
	nsvr_callback fnpointer = {};
	if (NSVR_Configuration_GetCallback(config, cbName.c_str(), &fnpointer)) {
		callbacks.callbacks->insert(std::make_pair(typeid(T).name(), fnpointer));
	}
}


int HardlightPlugin::Configure(nsvr_core_ctx* core)
{
	m_device.RegisterDrivers(core);
	//
	//nsvr_register_cevent_hook(core, nsvr_cevent_type::nsvr_cevent_brief_haptic, handle_event, )
	//m_device.RegisterDrivers([&](nsvr_cevent_handler consumer, const char* region, const char* iface, void* user_data) {
	//	nsvr_register_cevent_hook(core, nsvr_cevent_type::nsvr_cevent_basic_haptic
	//	m_coreApi.call<nsvr_core_register_node_cb>(consumer, region, iface, user_data);
	//});

	return 1;
}

