#include "stdafx.h"
#include "HardlightPlugin.h"
#include "PluginAPI.h"
#include <iostream>
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
	m_mockTracking(m_io->GetIOService(), boost::posix_time::millisec(16))

{
	
	m_adapter->SetMonitor(m_monitor);

	m_monitor->SetDisconnectHandler([&]() {
		m_coreApi["status"].call<NSVR_Core_StatusCallback>(false);
	});

	m_monitor->SetReconnectHandler([&]() {
		m_coreApi["status"].call<NSVR_Core_StatusCallback>(true);
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
		m_coreApi.at("tracking").call<NSVR_Core_TrackingCallback>(id.c_str(), &quat);
	});

	m_imus.AssignMapping(0x12, Imu::Chest, "chest");

	auto then = std::chrono::high_resolution_clock::now();
	m_mockTracking.SetEvent([&, then]() {
		auto s = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - then);
		packet fakeTrackingData;
		fakeTrackingData.raw[2] = 0x33;
		fakeTrackingData.raw[11] = 0x12;

		fakeTrackingData.raw[3] = s.count() & 0x0F;
		fakeTrackingData.raw[4] = s.count() & 0x0F;
		m_dispatcher.Dispatch(fakeTrackingData);
	
	});

	m_mockTracking.Start();
}

HardlightPlugin::~HardlightPlugin()
{
	m_eventPull.Stop();
	m_mockTracking.Stop();
	m_io->Shutdown();

}









int HardlightPlugin::Configure(NSVR_Configuration* config)
{
	auto functions = { "status", "register-node", "tracking" };

	for (const auto& func : functions) {
		nsvr_callback fnpointer = {};
		if (NSVR_Configuration_GetCallback(config, func, &fnpointer.callback, &fnpointer.context)) {
			m_coreApi.insert(std::make_pair(func, std::move(fnpointer)));
		}
	}

	m_device.RegisterDrivers([&](NSVR_Consumer_Handler_t consumer, const char* region, const char* iface, void* user_data) {
		m_coreApi.at("register-node")
			.call<NSVR_Core_RegisterNodeCallback>(consumer, region, iface, user_data);
	});

	return 1;
}

