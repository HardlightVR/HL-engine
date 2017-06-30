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
	m_eventPull(m_io->GetIOService(), boost::posix_time::milliseconds(5))

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
}

HardlightPlugin::~HardlightPlugin()
{
	m_eventPull.Stop();
	m_io->Shutdown();

}









int HardlightPlugin::Configure(NSVR_Configuration* config)
{
	auto functions = { "status", "register-node" };

	for (const auto& func : functions) {
		nsvr_callback fnpointer;
		NSVR_Configuration_GetCallback(config, func, &fnpointer.callback, &fnpointer.context);
		m_coreApi.insert(std::make_pair(func, std::move(fnpointer)));
	}

	m_device.RegisterDrivers([&](NSVR_Consumer_Handler_t consumer, const char* region, const char* iface, void* user_data) {
		m_coreApi.at("register-node")
			.call<NSVR_Core_RegisterNodeCallback>(consumer, region, iface, user_data);
	});

	return 1;
}

