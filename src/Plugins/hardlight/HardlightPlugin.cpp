#include "stdafx.h"
#include "HardlightPlugin.h"
#include "PluginAPI.h"
#include <iostream>
#include <typeinfo>
#include "PluginAPIWrapper.h"

nsvr_core* global_core = nullptr;

HardlightPlugin::HardlightPlugin(const std::string& data_dir) :
	m_io(std::make_shared<IoService>()),
	m_dispatcher(),
	m_adapter(std::make_unique<BoostSerialAdapter>(m_io->GetIOService())),
	m_firmware(data_dir, m_adapter.get(), m_io->GetIOService()),
	m_monitor(std::make_shared<KeepaliveMonitor>(m_io->GetIOService(), m_firmware)),
	m_synchronizer(std::make_unique<Synchronizer>(m_adapter->GetDataStream(), m_dispatcher, m_io->GetIOService())),
	m_device(),
	m_eventPull(m_io->GetIOService(), boost::posix_time::milliseconds(5)),
	m_imus(m_dispatcher),
	m_core{nullptr},
	m_trackingStream{nullptr}

{
	

	m_adapter->SetConnectionMonitor(m_monitor);

	m_monitor->OnReconnect([&]() {
		nsvr_device_event_raise(m_core, nsvr_device_event_device_connected, 0);
	});

	m_monitor->OnDisconnect([&]() {
		nsvr_device_event_raise(m_core, nsvr_device_event_device_disconnected, 0);
	});

	

	m_adapter->Connect();
	
	m_synchronizer->BeginSync();

	m_dispatcher.AddConsumer(PacketType::Ping, [this](const auto&) { m_monitor->ReceivePing(); });
	m_dispatcher.AddConsumer(PacketType::ImuData, [this](const auto&) { m_monitor->ReceivePing(); });


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

		}
	});

	
	m_imus.AssignMapping(0x3a, Imu::Chest, "chest"); 
	m_imus.AssignMapping(0x3c, Imu::Left_Upper_Arm, "left_upper_arm");

	



}

HardlightPlugin::~HardlightPlugin()
{
	m_eventPull.Stop();
	m_io->Shutdown();

}




struct bodygraph_region {
	nsvr_bodygraph_region* region;

	bodygraph_region() {
		nsvr_bodygraph_region_create(&region);
	}

	bodygraph_region& setLocation(nsvr_bodypart bodypart, double ratio, double rot) {
		nsvr_bodygraph_region_setlocation(region, bodypart, ratio, rot);
		return *this;
	}
	bodygraph_region& setDimensions(double width_cm, double height_cm) {
		nsvr_bodygraph_region_setboundingboxdimensions(region, width_cm, height_cm);
		return *this;
	}
	~bodygraph_region() {
		nsvr_bodygraph_region_destroy(&region);
		assert(region == nullptr);
	}
};

int HardlightPlugin::Configure(nsvr_core* core)
{

	


	m_core = core;
	global_core = core;
	m_device.Configure(core);

	nsvr_plugin_tracking_api tracking_api;
	tracking_api.beginstreaming_handler = [](nsvr_tracking_stream* stream, nsvr_node_id region, void* client_data) {
		AS_TYPE(HardlightPlugin, client_data)->BeginTracking(stream, region);
	};
	tracking_api.endstreaming_handler = [](nsvr_node_id region, void* client_data) {
		AS_TYPE(HardlightPlugin, client_data)->EndTracking(region);
	};
	tracking_api.client_data = this;
	nsvr_register_tracking_api(core, &tracking_api);
	

	nsvr_plugin_bodygraph_api body_api;
	body_api.setup_handler = [](nsvr_bodygraph* g, void* cd) {
		AS_TYPE(HardlightPlugin, cd)->SetupBodygraph(g);
	
	};
	body_api.client_data = this;
	nsvr_register_bodygraph_api(core, &body_api);

	nsvr_plugin_device_api device_api;
	device_api.client_data = this;
	device_api.enumeratenodes_handler = [](nsvr_device_id device_id, nsvr_node_ids* ids, void* cd) {
		AS_TYPE(HardlightPlugin, cd)->EnumerateNodesForDevice(device_id, ids);
	};
	device_api.enumeratedevices_handler = [](nsvr_device_ids* ids, void* cd) {
		AS_TYPE(HardlightPlugin, cd)->EnumerateDevices(ids);
	};
	device_api.getdeviceinfo_handler = [](nsvr_device_id id, nsvr_device_info* info, void* cd) {
		AS_TYPE(HardlightPlugin, cd)->GetDeviceInfo(id, info);
	};

	//Todo: GetNodeInfo should probably either take a device id, or nodes should be globaly unique. Think about this.
	device_api.getnodeinfo_handler = [](nsvr_node_id id, nsvr_node_info* info, void* cd) {
		AS_TYPE(HardlightPlugin, cd)->GetNodeInfo(id, info);
	};
	nsvr_register_device_api(core, &device_api);

	nsvr_directory dir = { 0 };
	nsvr_filesystem_getdatadirectory(core, &dir);


	nsvr_plugin_diagnostics_api diagnostics_api;
	diagnostics_api.client_data = this;
	diagnostics_api.updatemenu_handler = [](nsvr_diagnostics_ui* ui, void* cd) {
		AS_TYPE(HardlightPlugin,cd)->Render(ui);
	};

	nsvr_register_diagnostics_api(core, &diagnostics_api);
	
	return 1;
}

void HardlightPlugin::BeginTracking(nsvr_tracking_stream* stream, nsvr_node_id region)
{
	assert(region == 50); //chest_imu
	m_firmware.EnableTracking();
	m_trackingStream = stream;
}

void HardlightPlugin::EndTracking(nsvr_node_id region)
{
	m_firmware.DisableTracking();
	m_trackingStream = nullptr;
}

void HardlightPlugin::EnumerateNodesForDevice(nsvr_device_id, nsvr_node_ids * ids)
{
	m_device.EnumerateNodesForDevice(ids);
}

void HardlightPlugin::EnumerateDevices(nsvr_device_ids* ids)
{
	if (m_monitor->IsConnected()) {
		ids->device_count = 1;
		ids->ids[0] = 0;
	}
	else {
		ids->device_count = 0;
	}
}

void HardlightPlugin::GetDeviceInfo(nsvr_device_id id, nsvr_device_info * info)
{
	if (id == 0) {
	
		std::string device_name("Hardlight Suit");
		std::copy(device_name.begin(), device_name.end(), info->name);
		info->concept = nsvr_device_concept_suit;
	}
}

void HardlightPlugin::GetNodeInfo(nsvr_node_id id, nsvr_node_info* info)
{

	m_device.GetNodeInfo(id, info);
}

void HardlightPlugin::SetupBodygraph(nsvr_bodygraph * g)
{

	m_device.SetupDeviceAssociations(g);
	
}

void HardlightPlugin::Render(nsvr_diagnostics_ui * ui)
{
	static const std::vector<std::string> syncStates = {
		"Synchronized",
		"SearchingForSync",
		"ConfirmingSync",
		"ConfirmingSyncLoss"
	};
	ui->keyval("Serial port open", m_adapter->IsConnected() ? "true" : "false");
	ui->keyval("Confirmed connection to device", m_monitor->IsConnected()? "true" : "false");

	ui->keyval("Synchronizer state", syncStates[(int)m_synchronizer->SyncState()].c_str());
	
	
	if (ui->button("TRACKING_ENABLE")) {
		m_firmware.EnableTracking();
	}
	if (ui->button("TRACKING_DISABLE")) {
		m_firmware.DisableTracking();
	}



	ui->keyval("Total bytes sent", std::to_string(m_firmware.GetTotalBytesSent()).c_str());
	ui->keyval("Total bytes rec'd", std::to_string(m_synchronizer->GetTotalBytesRead()).c_str());
	
}

