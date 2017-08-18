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
		
		//todo: there is a difference between devices and nodes. I have let this confusion slide.
		//Now it must be reckoned with. 
		//todo: think about over vacation
		nsvr_device_event_raise(m_core, nsvr_device_event_device_disconnected, 1);
	
	});

	m_monitor->OnReconnect([&]() {
		nsvr_device_event_raise(m_core, nsvr_device_event_device_connected, 1);

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
	tracking_api.beginstreaming_handler = [](nsvr_tracking_stream* stream, nsvr_region region, void* client_data) {
		AS_TYPE(HardlightPlugin, client_data)->BeginTracking(stream, region);
	};
	tracking_api.endstreaming_handler = [](nsvr_region region, void* client_data) {
		AS_TYPE(HardlightPlugin, client_data)->EndTracking(region);
	};
	tracking_api.client_data = this;
	nsvr_register_tracking_api(core, &tracking_api);
	

	nsvr_plugin_bodygraph_api body_api;
	body_api.setup_handler = [](nsvr_bodygraph* g, void* cd) {
		
		//Grab the top parallel, which runs right across the chest
		nsvr_parallel para;
		nsvr_parallel_init(&para, nsvr_bodypart_torso, nsvr_parallel_highest);
		
		nsvr_bodygraph_region* chestLocation;
		nsvr_bodygraph_region_create(&chestLocation);
		//Chest left is on the front of the body, offset by a bit
		nsvr_bodygraph_region_setorigin(chestLocation, &para, nsvr_bodypart_rotation_front - 10);

		//Also set the width of the zone (and height later)
		nsvr_bodygraph_region_setwidthcm(chestLocation, 10);

		nsvr_bodygraph_createnode_absolute(g, "Chest_Left", chestLocation);
		nsvr_bodygraph_createnode_relative(g, "Upper_Ab_Left", nsvr_region_relation_below, "Chest_Left");
		nsvr_bodygraph_createnode_relative(g, "Mid_Ab_Left", nsvr_region_relation_below, "Upper_Ab_Left");
		nsvr_bodygraph_createnode_relative(g, "Lower_Ab_Left", nsvr_region_relation_below, "Mid_Ab_Left");

		nsvr_bodygraph_connect(g, "Chest_Left", "Upper_Ab_Left");
		nsvr_bodygraph_connect(g, "Upper_Ab_Left", "Mid_Ab_Left");
		nsvr_bodygraph_connect(g, "Mid_Ab_Left", "Lower_Ab_Left");

		//Chest right is on the front of the body, offset by a bit
		nsvr_bodygraph_region_setorigin(chestLocation, &para, nsvr_bodypart_rotation_front + 10);
		nsvr_bodygraph_createnode_absolute(g, "Chest_Right", chestLocation);
		nsvr_bodygraph_createnode_relative(g, "Upper_Ab_Right", nsvr_region_relation_below, "Chest_Right");
		nsvr_bodygraph_createnode_relative(g, "Mid_Ab_Right", nsvr_region_relation_below, "Upper_Ab_Right");
		nsvr_bodygraph_createnode_relative(g, "Lower_Ab_Right", nsvr_region_relation_below, "Mid_Ab_Right");

		nsvr_bodygraph_connect(g, "Chest_Right", "Upper_Ab_Right");
		nsvr_bodygraph_connect(g, "Upper_Ab_Right", "Mid_Ab_Right");
		nsvr_bodygraph_connect(g, "Mid_Ab_Right", "Lower_Ab_Right");

		nsvr_bodygraph_connect(g, "Chest_Left", "Chest_Right");
		nsvr_bodygraph_connect(g, "Upper_Ab_Right", "Upper_Ab_Left");
		nsvr_bodygraph_connect(g, "Mid_Ab_Left", "Mid_Ab_Right");
		nsvr_bodygraph_connect(g, "Lower_Ab_Right", "Lower_Ab_Left");


		nsvr_bodygraph_putdevice(g, "Chest_Left", 3);
		nsvr_bodygraph_region_destroy(&chestLocation);

	};
	body_api.client_data = this;
	nsvr_register_bodygraph_api(core, &body_api);
	return 1;
}

void HardlightPlugin::BeginTracking(nsvr_tracking_stream* stream, nsvr_region region)
{
	m_firmware.EnableTracking();
	m_trackingStream = stream;
}

void HardlightPlugin::EndTracking(nsvr_region region)
{
	m_firmware.DisableTracking();
	m_trackingStream = nullptr;
}

