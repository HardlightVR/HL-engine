#include "stdafx.h"
#include "HardlightPlugin.h"
#include "PluginAPI.h"
#include <iostream>
#include <typeinfo>
#include "PluginAPIWrapper.h"
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
	m_core{nullptr},
	m_trackingStream{nullptr}

{
	

	m_adapter->SetMonitor(m_monitor);

	m_monitor->OnDisconnect([&]() {
		//nsvr::Event event{nsvr_device_event_device_disconnected};
		//event.raise(m_core);

		nsvr_device_event* event;
		nsvr_device_event_create(&event, nsvr_device_event_device_disconnected);
		nsvr_device_event_raise(m_core, event);
		nsvr_device_event_destroy(&event);
	});

	m_monitor->OnReconnect([&]() {
		nsvr_device_event* event;
		nsvr_device_event_create(&event, nsvr_device_event_device_connected);
		nsvr_device_event_raise(m_core, event);
		nsvr_device_event_destroy(&event);
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

	m_imus.OnTracking([&](const std::string& id, nsvr_quaternion quat) {

		if (m_trackingStream != nullptr) {

			nsvr_tracking_stream_push(m_trackingStream, &quat);

		/*	nsvr_device_event* event = nullptr;
			nsvr_device_event_create(&event, nsvr_device_event_tracking_update);
			nsvr_device_event_setid(event, 1);
			nsvr_device_event_settrackingstate(event, id.c_str(), &quat);
			nsvr_device_event_raise(m_core, event);
			nsvr_device_event_destroy(&event);*/
		}
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






int HardlightPlugin::Configure(nsvr_core* core)
{
	m_core = core;
	m_device.Configure(core);

	nsvr_plugin_tracking_api tracking_api;
	tracking_api.beginstreaming_handler = [](nsvr_tracking_stream* stream, const char* region, void* client_data) {
		AS_TYPE(HardlightPlugin, client_data)->BeginTracking(stream, region);
	};
	tracking_api.endstreaming_handler = [](const char* region, void* client_data) {
		AS_TYPE(HardlightPlugin, client_data)->EndTracking(region);
	};
	tracking_api.client_data = this;
	nsvr_register_tracking_api(core, &tracking_api);
	

	return 1;
}

void HardlightPlugin::BeginTracking(nsvr_tracking_stream* stream, const char* region)
{
	m_firmware.EnableTracking();
	m_trackingStream = stream;
}

void HardlightPlugin::EndTracking(const char* region)
{
	m_firmware.DisableTracking();
	m_trackingStream = nullptr;
}

