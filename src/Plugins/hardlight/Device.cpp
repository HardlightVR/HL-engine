#include "stdafx.h"
#include "Device.h"

#include "IoService.h"
#include "Heartbeat.h"

#include <typeinfo>
#include "PluginAPIWrapper.h"
#include "IMU_ID.h"
#include "DeviceManager.h"

#include "synchronizer2.h"
#include "Instructions.h"
#include "zone_logic/HardwareCommands.h"
nsvr_core* global_core = nullptr;
//note: can make firmware unique
Device::Device(boost::asio::io_service& io, const std::string& data_dir, std::unique_ptr<PotentialDevice> device, hardlight_device_version version) :
	m_core{ nullptr },
	m_io(io),
	m_hwIO(std::move(device->io)),
	m_firmware(std::make_shared<FirmwareInterface>(data_dir, m_hwIO->outgoing_queue(), m_io)),
	m_device(*m_firmware->GetInstructions()),
	m_monitor(std::make_shared<Heartbeat>(m_io, m_firmware)),
	m_synchronizer(device->synchronizer),
	m_dispatcher(device->dispatcher),
	m_running(),
	m_imus(*m_dispatcher),
	m_version(version),
	m_motors(),
	m_deferredCommands(512)

{
	


	
	

	m_dispatcher->AddConsumer(inst::Id::GET_PING, [this](const auto&) { m_monitor->ReceiveResponse(); });
	m_dispatcher->AddConsumer(inst::Id::GET_TRACK_DATA, [this](const auto&) { m_monitor->ReceiveResponse(); });
	
	m_dispatcher->AddConsumer(inst::Id::GET_MOTOR_STATUS, [this](const Packet& packet) {
		auto status = packet[3];
		auto motor = packet[4];
		m_motors[motor] = MotorStatus(motor, static_cast<HL_Unit::_enumerated>(status));
	

		core_log(nsvr_severity_info, "Device", "Got motor status update");
	});


	
	m_imus.AssignMapping(0x3c, Imu::Chest, NODE_IMU_CHEST); 
	m_imus.AssignMapping(0x39, Imu::Right_Upper_Arm, NODE_IMU_RIGHT_UPPER_ARM);
	m_imus.AssignMapping(0x3a, Imu::Left_Upper_Arm, NODE_IMU_LEFT_UPPER_ARM);

	
	
	for (int i = static_cast<int>(Location::Lower_Ab_Right); i < static_cast<int>(Location::Error); i++) {
		m_firmware->EnableIntrigMode(static_cast<Location>(i));
	}


}

Device::~Device()
{
	std::cout << "DESTROYING HARDLIGHT DEVICE\n";
	m_dispatcher->ClearConsumers();
	m_synchronizer->stop();
	m_hwIO->stop();

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

float Device::GetIoUtilizationRatio() const
{
	return (float) m_hwIO->outgoing_queue_size() / (float) m_hwIO->outgoing_queue_capacity();
}

int Device::Configure(nsvr_core* core)
{

	


	m_core = core;
	global_core = core;
	m_device.Configure(core);

	nsvr_plugin_tracking_api tracking_api;
	tracking_api.beginstreaming_handler = [](nsvr_tracking_stream* stream, nsvr_node_id region, void* client_data) {
		AS_TYPE(Device, client_data)->BeginTracking(stream, region);
	};
	tracking_api.endstreaming_handler = [](nsvr_node_id region, void* client_data) {
		AS_TYPE(Device, client_data)->EndTracking(region);
	};
	tracking_api.client_data = this;
	nsvr_register_tracking_api(core, &tracking_api);
	

	nsvr_plugin_bodygraph_api body_api;
	body_api.setup_handler = [](nsvr_bodygraph* g, void* cd) {
		AS_TYPE(Device, cd)->SetupBodygraph(g);
	
	};
	body_api.client_data = this;
	nsvr_register_bodygraph_api(core, &body_api);


	nsvr_plugin_analogaudio_api analog;
	analog.client_data = this;
	analog.open_handler = [](nsvr_node_id node, void* cd) {
		AS_TYPE(Device, cd)->PushDeferred(EnableAudio{ static_cast<Location>(node), { 0x00, 0x00, 0x00,7, 38 } });
	};
	analog.close_handler = [](nsvr_node_id node, void* cd) {
		AS_TYPE(Device, cd)->PushDeferred(DisableAudio{ static_cast<Location>(node) });
	};

	nsvr_register_analogaudio_api(core, &analog);
	
	return 1;
}

void Device::BeginTracking(nsvr_tracking_stream* stream, nsvr_node_id id)
{
	m_imus.AssignStream(stream, id);
	m_firmware->EnableTracking();
}

void Device::EndTracking(nsvr_node_id id)
{
	m_imus.RemoveStream(id);
	m_firmware->DisableTracking();
}

void Device::EnumerateNodesForDevice(nsvr_node_ids * ids)
{
	m_device.EnumerateNodesForDevice(ids);
}

void Device::EnumerateDevices(nsvr_device_ids* ids)
{
	if (m_monitor->IsConnected()) {
		ids->device_count = 1;
		ids->ids[0] = 0;
	}
	else {
		ids->device_count = 0;
	}
}

void Device::GetDeviceInfo(nsvr_device_info * info)
{
	static std::unordered_map<int, std::string> products = {{2, "MarkII"}, {3, "MarkIII" }};

	std::string mark = "(Unknown Version)";
	if (products.find(m_version.product) != products.end()) {
		mark = products.at(m_version.product);
	}

	std::string device_name("Hardlight Suit " + mark);
	std::copy(device_name.begin(), device_name.end(), info->name);
	info->concept = nsvr_device_concept_suit;
	
}

void Device::GetNodeInfo(nsvr_node_id id, nsvr_node_info* info)
{

	m_device.GetNodeInfo(id, info);
}

void Device::SetupBodygraph(nsvr_bodygraph * g)
{

	m_device.SetupDeviceAssociations(g);
	
}
std::vector<MotorStatus> Device::GetMotorStatus() const
{
	std::vector<MotorStatus> statuses;
	for (const auto& kvp : m_motors) {
		statuses.push_back(kvp.second);
	}
	return statuses;
}
std::string stringifyStatusBits(HL_Unit status) {
	std::stringstream ss;
	for (auto val : HL_Unit::_values()) {
		if (status & val) {
			ss << val._to_string() << "|";
		}
	}
	return ss.str();
}

void Device::Render(nsvr_diagnostics_ui * ui)
{
	static const std::vector<std::string> syncStates = {
		"Synchronized",
		"SearchingForSync",
		"ConfirmingSync",
		"ConfirmingSyncLoss"
	};
	
	ui->keyval("Product version", std::to_string(m_version.product).c_str());
	ui->keyval("Product revision", std::to_string(m_version.revision).c_str());
	ui->keyval("Firmware major", std::to_string(m_version.firmware_a).c_str());
	ui->keyval("Firmware minor", std::to_string(m_version.firmware_b).c_str());


	ui->keyval("Synchronizer state", syncStates[(int)m_synchronizer->state()].c_str());
	
	
	if (ui->button("TRACKING_ENABLE")) {
		m_firmware->EnableTracking();
	}
	if (ui->button("TRACKING_DISABLE")) {
		m_firmware->DisableTracking();
	}

	if (ui->button("GET_TRACK_STATUS")) {
		m_firmware->RequestTrackingStatus();
	}
	
	auto imuInfo = m_imus.GetInfo();

	bool all_imu_good = std::all_of(imuInfo.begin(), imuInfo.end(), [](const ImuInfo& info) { return info.status == 141; });

	ui->keyval("All imus okay?" , all_imu_good ? "true" : "false");
	for (const auto& imu : imuInfo) {

		std::string imuStr = Locator::Translator().ToString(static_cast<Imu>(imu.friendlyName));

		std::string imuId("Imu " + std::to_string((int)imu.firmwareId));
		std::string friendlyId("(" + imuStr + ")");
		ui->keyval(imuId.c_str(), friendlyId.c_str());
		ui->keyval("status", stringifyStatusBits(imu.status).c_str());
	}

	if (ui->button("GET_MOTOR_STATUS")) {
		for (int i = static_cast<int>(Location::Lower_Ab_Right); i < static_cast<int>(Location::Error); i++) {
			m_firmware->GetMotorStatus(static_cast<Location>(i));
		}
	}
	
	std::vector<bool> statuses;
	for (int i = static_cast<int>(Location::Lower_Ab_Right); i < static_cast<int>(Location::Error); i++) {

		std::string pad = Locator::Translator().ToString(static_cast<Location>(i));
		uint8_t actualZone = m_firmware->GetInstructions()->ParamDict().at("zone").at(pad);
		auto it = m_motors.find(actualZone);
		if (it != m_motors.end()) {
			if (it->second.Status == 141) {
				statuses.push_back(true);
			}
			else {
				statuses.push_back(false);
			}
		}
		else {
			statuses.push_back(false);
		}
	}

	bool all_good = std::all_of(statuses.begin(), statuses.end(), [](bool a) { return a; });
	ui->keyval("All motors okay?", all_good ? "true" : "false");


	for (int i = static_cast<int>(Location::Lower_Ab_Right); i < static_cast<int>(Location::Error); i++) {
		std::string pad = Locator::Translator().ToString(static_cast<Location>(i));
		std::string status = "[no data]";

		uint8_t actualZone = m_firmware->GetInstructions()->ParamDict().at("zone").at(pad);
		std::string motorName = "Motor " + std::to_string(actualZone);

		
		auto it = m_motors.find(actualZone);

		if (it != m_motors.end()) {
			status = stringifyStatusBits(it->second.Status);
			
		}

		std::string friendly("(" + pad + ")");
		ui->keyval(motorName.c_str(), friendly.c_str());
		ui->keyval("status", status.c_str());
	}



	/*static AudioOptions opts{ 0x00, 0x00, 0x00,7, 38 };

	if (ui->slider_int("VibeCtrl", &opts.VibeCtrl, 0, 4)	||
		ui->slider_int("AudioMin", &opts.AudioMin, 0, 255)	||
		ui->slider_int("AudioMax", &opts.AudioMax, 0, 255)	||
		ui->slider_int("MinDrv", &opts.MinDrv, 0, 255)		||
		ui->slider_int("MaxDrv", &opts.MaxDrv, 0, 255))	
	{
		
	}
*/
	
	


	static int rtpVol = 0;
	if (ui->button("Enable RTP Mode on all")) {
		for (int i = static_cast<int>(Location::Lower_Ab_Right); i < static_cast<int>(Location::Error); i++) {
			m_firmware->EnableRtpMode(static_cast<Location>(i));
		}
	}

	if (ui->button("Disable RTP Mode on all")) {
		for (int i = static_cast<int>(Location::Lower_Ab_Right); i < static_cast<int>(Location::Error); i++) {
			m_firmware->EnableIntrigMode(static_cast<Location>(i));
		}
	}
	if (ui->slider_int("RTP", &rtpVol, 0, 127)) {
		for (int i = static_cast<int>(Location::Lower_Ab_Right); i < static_cast<int>(Location::Error); i++) {
			m_firmware->PlayRtp(static_cast<Location>(i), rtpVol);
		}
	}

	

	ui->keyval("Zombie mode test", "(drag slider to change # of clicks)");
	if (ui->button("First enable intrig mode on chest_left")) {
		m_firmware->EnableIntrigMode(Location::Chest_Left);
	}
	static int howmany = 0;
	ui->slider_int("", &howmany, 0, 16);
	
	auto buttontext = std::string("Send " + std::to_string(howmany) + " clicks to chest_left");
	if (ui->button(buttontext.c_str())) {
		for (int i = 0; i < howmany; i++) {
			m_firmware->PlayEffect(Location::Chest_Left, 3, 1.0);
		}
	}

	 int queue_size = m_hwIO->outgoing_queue_size();
	ui->slider_int("Outgoing queue", &queue_size, 0, m_hwIO->outgoing_queue_capacity());

	ui->keyval("Total bytes sent", std::to_string(m_hwIO->bytes_written()).c_str());
	ui->keyval("Total bytes rec'd", std::to_string(m_hwIO->bytes_read()).c_str());
	
}

void Device::Update()
{
	constexpr auto ms_fraction_of_second = (1.0f / 1000.f);
	auto dt = 20 * ms_fraction_of_second;

	
	auto commands = m_device.GenerateHardwareCommands(dt);
	m_firmware->Execute(commands);


	CommandBuffer singleDeferredCommand;
	m_deferredCommands.consume_one([&](auto cmd) {
		singleDeferredCommand.push_back(cmd);
	});

	m_firmware->Execute(singleDeferredCommand);

}

void Device::PushDeferred(FirmwareCommand command)
{
	m_deferredCommands.push(command);
}



