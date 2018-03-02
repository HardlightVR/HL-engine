#include "stdafx.h"
#include "DeviceManager.h"
#include <iostream>
#include "Device.h "
#include "hardlight_device_version.h"
#include "AsyncPacketRequest.h"
#include "Writer.h"
#include "WifiConnector.h"
#include "JsonKeyValueConfig.h"

using SerialIO = IoBase<
	boost::asio::serial_port,
	SerialPortReader, //todo: make Reader generic 
	Writer<boost::asio::serial_port>,
	wired_connection,
	SerialPortConnector
>;

using WifiIO = IoBase<
	boost::asio::ip::tcp::socket,
	SocketReader, //todo: make Reader generic 
	Writer<boost::asio::ip::tcp::socket>,
	wifi_connection,
	WifiConnector
>;


static std::array<uint8_t, 16> version_packet = { 0x24,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x0D,0x0A };
static std::array<uint8_t, 16> uuid_packet = { 0x24,0x02,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x0D,0x0A };

void requestSuitVersion(boost::lockfree::spsc_queue<uint8_t>& output)
{
	output.push(version_packet.data(), version_packet.size());
}

void requestUuid(boost::lockfree::spsc_queue<uint8_t>& output)
{
	output.push(uuid_packet.data(), uuid_packet.size());
}

bool validateWifiConfig(const std::unordered_map<std::string, std::string>& wifiConfig) {
	bool valid = true;
	
	if (wifiConfig.find("host") == wifiConfig.end()) {
		core_log(nsvr_severity_warning, "DeviceManager", "Could not load Wifi configuration: 'host' must be present");
		valid = false;
	}
	if (wifiConfig.find("port") == wifiConfig.end()) {
		core_log(nsvr_severity_warning, "DeviceManager", "Could not load Wifi configuration: 'port' must be present");
		valid = false;
	}
	if (wifiConfig.find("password") == wifiConfig.end()) {
		core_log(nsvr_severity_warning, "DeviceManager", "Could not load Wifi configuration: 'password' must be present");
		valid = false;
	}

	return valid;

}

DeviceManager::DeviceManager(std::string path)
	: m_ioService()
	, m_path(path)
	, m_deviceIds()
	, m_doctor(m_ioService.GetIOService())
	, m_recognizer(m_ioService.GetIOService(), &m_doctor)
	, m_requestVersionTimeout(boost::posix_time::millisec(200))
	, m_requestVersionTimer(m_ioService.GetIOService())
	, m_idPool()
	, m_devicePollTimeout(boost::posix_time::millisec(5))
	, m_devicePollTimer(m_ioService.GetIOService())
	, m_deviceLock()
{
	


	m_recognizer.on_recognize([this](connection_info info) {
		//dispatch the event on our own IO service's thread (it came from the serial_connection_manager's thread)
		m_ioService.GetIOService().post([this, info]() { 
			handle_recognize(info);
		});
	});

	m_recognizer.on_unrecognize([this](std::string interface_name) {
		//dispatch the event on our own IO service's thread (it came from the serial_connection_manager's thread)
		m_ioService.GetIOService().post([this, interface_name]() {
			handle_unrecognize(interface_name);
		});
	});
	
	m_recognizer.start();


	m_devicePollTimer.expires_from_now(m_devicePollTimeout);
	m_devicePollTimer.async_wait([this](auto ec) { if (ec) { return; } 
		update();
	});



	//Not a good place to do this loading and validating at all, but we are trying to get v1 of wifi out!
	//Load the configuration object from disk. If it is loaded, then immediately try to connect to it.
	//Else, we would be looking for serial port connections as normal.
	try {
		std::unordered_map<std::string, std::string> wifiConfig =
			nsvr::tools::json::parseDictFromDict<std::string, std::string>(m_path + "/Wifi.json",
				[](const Json::Value& key) {
			return key.asString();
		},
				[](const Json::Value& value) {
			return value.asString();
		}
		);

		if (validateWifiConfig(wifiConfig)) {
			m_recognizer.recognize(wifi_connection{ wifiConfig["host"], wifiConfig["port"], wifiConfig["password"] });
		}
	}
	catch (...) {
		core_log(nsvr_severity_warning, "DeviceManager", "Could not load Wifi configuration. Harmless if you aren't using Wifi.");
	}
}

class create_io_object : public boost::static_visitor<std::unique_ptr<HardwareIO>> {
public:
	create_io_object(boost::asio::io_service& io) : m_io(io){

	}
	std::unique_ptr<HardwareIO> operator()(wired_connection conn) const {
		
		auto port = std::make_unique<boost::asio::serial_port>(m_io);

		return std::make_unique<SerialIO>(std::move(port), conn);
	}
	std::unique_ptr<HardwareIO> operator()(wifi_connection conn) const {

		using tcp = boost::asio::ip::tcp;

		auto socket = std::make_unique<tcp::socket>(m_io);

		return std::make_unique<WifiIO>(std::move(socket), conn);
	}
private:
	boost::asio::io_service& m_io;
};

void DeviceManager::handle_recognize(connection_info info)
{
	auto device = boost::apply_visitor(create_io_object(m_ioService.GetIOService()), info);
	if (!device) {
		return;
	}

	auto device_name = boost::apply_visitor(get_interface_name(), info);
	m_potentials[device_name] = std::move(device);
}

void DeviceManager::handle_unrecognize(std::string interface_name)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);

	auto it = std::find_if(m_deviceIds.begin(), m_deviceIds.end(), [port = interface_name](const auto& kvp) { return kvp.second == port; });

	if (it != m_deviceIds.end()) {
		m_doctor.release_patient();

		nsvr_device_event_raise(m_core, nsvr_device_event_device_disconnected, it->first);
		m_idPool.Release(it->first);

		m_devices.erase(it->second);
		m_deviceIds.erase(it);
	}




}



class doctor_report_visitor : public boost::static_visitor<void> {
public:
	doctor_report_visitor() : status(0), error(0) {}
	void operator()(Doctor::Status status) {
		error = 0;
		this->status = static_cast<int>(status);
	}

	void operator()(HardwareFailures failure) {
		status = 0;
		error = static_cast<uint64_t>(failure);
	}

	void query(int* outstatus, uint64_t* outerror) {
		*outstatus = this->status;
		*outerror = this->error;
	}
private:
	int status;
	uint64_t error;
};
void DeviceManager::GetCurrentDeviceState(int * status, uint64_t* error)
{
	doctor_report_visitor results{};
	boost::apply_visitor(results, m_doctor.query_patient());
	results.query(status, error);
}

void DeviceManager::create_device(const std::string& deviceName) {

	//Establish a mapping between numeric device ID and the device name (derived from port, or host)
	auto id = m_idPool.Request();
	m_deviceIds[id] = deviceName;

	std::unique_ptr<HardwareIO> potential = std::move(m_potentials.at(deviceName));
	//m_doctor.accept_patient(id, potential.get());

	auto fullDevice = std::make_unique<Device>(
		m_ioService.GetIOService(), 
		m_path, 
		std::move(potential), 
		potential->GetVersion()
	);

	fullDevice->Configure(m_core);

	m_devices[deviceName] = std::move(fullDevice);

	m_ioService.GetIOService().post([core = m_core, device_id = id]() {
		nsvr_device_event_raise(core, nsvr_device_event_device_connected, device_id);
	});
}


void DeviceManager::update_each_device() {
	for (auto& kvp : m_devices) {
		kvp.second->Update();
	}
}

std::vector<std::string> DeviceManager::get_newly_connected_devices() const {
	std::vector<std::string> ready;
	for (const auto& kvp : m_potentials) {
		if (kvp.second->ready()) {
			ready.push_back(kvp.first);
		}
	}
	return ready;
}

float DeviceManager::compute_device_io_utilization() const {
	float util_ratio = 0.0;
	for (auto& kvp : m_devices) {
		util_ratio = std::max(util_ratio, kvp.second->GetIoUtilizationRatio());
	}
	return util_ratio;
}


void DeviceManager::schedule_update() {
	m_devicePollTimer.expires_from_now(m_devicePollTimeout);
	m_devicePollTimer.async_wait([this](auto ec) { 
		if (ec) { return; }
		update();
	});
}
void DeviceManager::update()
{
	std::lock_guard<std::mutex> guard(m_deviceLock);
	
	update_each_device();

	auto util_ratio = compute_device_io_utilization();
	m_devicePollTimeout = boost::posix_time::millisec((60 * std::pow(util_ratio, 2.71f)) + 10);


	auto newlyConnectedDevices = get_newly_connected_devices();
	for (const auto& device : newlyConnectedDevices) {
		create_device(device);
		m_potentials.erase(device);
	}


	schedule_update();

}



DeviceManager::~DeviceManager()
{
	m_recognizer.stop();
	m_devicePollTimer.cancel();
}



int DeviceManager::configure(nsvr_core * core)
{
	global_core = core;
	m_core = core;

	nsvr_plugin_device_api device_api;
	device_api.client_data = this;
	device_api.enumeratenodes_handler = [](nsvr_device_id device_id, nsvr_node_ids* ids, void* cd) {
		AS_TYPE(DeviceManager, cd)->EnumerateNodesForDevice(device_id, ids);
	};
	device_api.enumeratedevices_handler = [](nsvr_device_ids* ids, void* cd) {
		AS_TYPE(DeviceManager, cd)->EnumerateDevices(ids);
	};
	device_api.getdeviceinfo_handler = [](nsvr_device_id id, nsvr_device_info* info, void* cd) {
		AS_TYPE(DeviceManager, cd)->GetDeviceInfo(id, info);
	};
	device_api.getnodeinfo_handler = [](nsvr_device_id did, nsvr_node_id id, nsvr_node_info* info, void* cd) {
		AS_TYPE(DeviceManager, cd)->GetNodeInfo(did, id, info);
	};
	nsvr_register_device_api(core, &device_api);

	nsvr_plugin_diagnostics_api diagnostics_api;
	diagnostics_api.client_data = this;
	diagnostics_api.updatemenu_handler = [](nsvr_diagnostics_ui* ui, void* cd) {
		AS_TYPE(DeviceManager, cd)->Render(ui);
	};

	nsvr_register_diagnostics_api(core, &diagnostics_api);



	nsvr_plugin_verification_api verification_api;
	verification_api.client_data = this;
	verification_api.getcurrentdevicestate_handler = [](int* outStatus, uint64_t* outError, void* cd) {
		AS_TYPE(DeviceManager, cd)->GetCurrentDeviceState(outStatus, outError);
	};
	nsvr_register_verification_api(core, &verification_api);
	return 1;
}

void DeviceManager::Render(nsvr_diagnostics_ui* ui) {
	std::lock_guard<std::mutex> guard(m_deviceLock);


	for (auto& device : m_devices) {
		device.second->Render(ui);
	}
}
void DeviceManager::EnumerateNodesForDevice(nsvr_device_id id, nsvr_node_ids * ids)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);


	auto portName = m_deviceIds.at(id);
	m_devices.at(portName)->EnumerateNodesForDevice(ids);
}

void DeviceManager::EnumerateDevices(nsvr_device_ids * ids)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);


	ids->device_count = m_deviceIds.size();

	std::size_t index = 0;
	for (const auto& id : m_deviceIds) {
		ids->ids[index++] = id.first;
	}
}

void DeviceManager::GetDeviceInfo(nsvr_device_id id, nsvr_device_info * info)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);


	auto portName = m_deviceIds.at(id);
	m_devices.at(portName)->GetDeviceInfo(info);
}

void DeviceManager::GetNodeInfo(nsvr_device_id device_id, nsvr_node_id node_id, nsvr_node_info * info)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);

	auto portName = m_deviceIds.at(device_id);
	m_devices.at(portName)->GetNodeInfo(node_id, info);
}
