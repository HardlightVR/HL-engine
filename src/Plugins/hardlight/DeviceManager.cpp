#include "stdafx.h"
#include "DeviceManager.h"
#include <iostream>
#include "Device.h "
#include "hardlight_device_version.h"
#include "AsyncPacketRequest.h"
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

DeviceManager::DeviceManager(std::string path)
	: m_ioService()
	, m_path(path)
	, m_deviceIds()
	, m_doctor()
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

	m_recognizer.on_unrecognize([this](connection_info info) {
		//dispatch the event on our own IO service's thread (it came from the serial_connection_manager's thread)
		m_ioService.GetIOService().post([this, info]() {
			handle_unrecognize(info);
		});
	});
	
	m_recognizer.start();


	m_devicePollTimer.expires_from_now(m_devicePollTimeout);
	m_devicePollTimer.async_wait([this](auto ec) { if (ec) { return; } 
		device_update();
	});
}


void DeviceManager::handle_recognize(connection_info info)
{
	auto port = std::make_unique<boost::asio::serial_port>(m_ioService.GetIOService());
	
	boost::system::error_code ec;
	port->open(info.port_name, ec);
	if (!port->is_open()) {
		core_log(nsvr_severity_error, "DeviceManager", "Unable to re-open the previously detected port " + info.port_name);
		return;
	}

	auto device = std::make_unique<PotentialDevice>(std::move(port));
	
	//begins reading and writing to port
	device->io->start();

	//begins synchronizing incoming packets from the device
	device->synchronizer->start();

	//in order for a device to move from "potentially found" to "actually present", we need it to return its version
	device->dispatcher->AddConsumer(inst::Id::GET_VERSION, [this, portName = info.port_name](auto packet) {
		handle_connect(portName, packet);
	});

	//some suits, or a zombie suit, may end up streaming tracking data on connect. We should tell the user to reset the suit if this happens.
	device->dispatcher->AddConsumer(inst::Id::GET_TRACK_DATA, [this, portName = info.port_name](auto packet) {
		core_log(nsvr_severity_warning, "DeviceManager", "It seems that a device might be connected on " + portName + ", but can't confirm. Please power cycle the device.");
	});


	device->synchronizer->on_packet([weakDispatch = std::weak_ptr<PacketDispatcher>(device->dispatcher)](auto packet) {
		if (auto dispatcher = weakDispatch.lock()) {
			dispatcher->Dispatch(std::move(packet));
		}
	});

	requestSuitVersion(*device->io->outgoing_queue());
	requestSuitVersion(*device->io->outgoing_queue());
	requestUuid(*device->io->outgoing_queue());

	m_potentials.insert(std::make_pair(info.port_name, std::move(device)));
}

void DeviceManager::handle_unrecognize(connection_info info)
{
	std::lock_guard<std::mutex> guard(m_deviceLock);

	auto it = std::find_if(m_deviceIds.begin(), m_deviceIds.end(), [port = info.port_name](const auto& kvp) { return kvp.second == port; });

	if (it != m_deviceIds.end()) {
		m_doctor.notify_device_status(it->first, Doctor::Status::Unplugged);

		nsvr_device_event_raise(m_core, nsvr_device_event_device_disconnected, it->first);
		m_idPool.Release(it->first);

		m_devices.erase(it->second);
		m_deviceIds.erase(it);
	}




}


void DeviceManager::GetCurrentDeviceState(int * outState)
{
	auto devices = m_doctor.get_devices();

	if (devices.empty()) {
		*outState = 2; //Unplugged
		return;
	}

	auto status = m_doctor.get_device_status(devices[0]);


	if (status == Doctor::Status::OkDiagnostics) {
		*outState = 1; //Ok
		return;
	}

	if (status == Doctor::Status::CheckingDiagnostics) {
		*outState = 4; //Checking
		return;
	}

	if (status == Doctor::Status::Unplugged) {
		*outState = 2;
		return;
	}

	if (status < Doctor::Status::Unknown) {
		*outState = static_cast<int>(status); // error
		return;
	}


	*outState = 0; // unknown
	
}

void DeviceManager::handle_connect(std::string portName, Packet versionPacket) {

	auto id = m_idPool.Request();
	m_deviceIds[id] = portName;
	{
		std::lock_guard<std::mutex> guard(m_deviceLock);

		if (m_potentials.find(portName) == m_potentials.end()) {
			return;
		}

		auto version = parse_version(versionPacket);
		
		auto potential = std::move(m_potentials.at(portName));

		m_potentials.erase(portName);
		

		potential->dispatcher->ClearConsumers();

		std::make_shared<diagnostics>(
			doctor, 
			version, 
			potential->io.get(), 
			potential->dispatcher)
		->begin();



		auto real = std::make_unique<Device>(m_ioService.GetIOService(), m_path, std::move(potential), version);
		real->Configure(m_core);


		m_devices.insert(std::make_pair(portName, std::move(real)));


	}

	
	m_doctor.notify_device_status(id, Doctor::Status::CheckingDiagnostics);


	nsvr_device_event_raise(m_core, nsvr_device_event_device_connected, id);


}

void DeviceManager::device_update()
{
	std::lock_guard<std::mutex> guard(m_deviceLock);

	float util_ratio = 0.0;
	for (auto& kvp : m_devices) {
		kvp.second->Update();
		util_ratio = std::max(util_ratio, kvp.second->GetIoUtilizationRatio());
	}

	m_devicePollTimeout = boost::posix_time::millisec((45 * std::pow(util_ratio, 2.71f)) + 10);

	m_devicePollTimer.expires_from_now(m_devicePollTimeout);
	m_devicePollTimer.async_wait([this](auto ec) { if (ec) { return; }
		device_update();
	});

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
	verification_api.getcurrentdevicestate_handler = [](int* outState, void* cd) {
		AS_TYPE(DeviceManager, cd)->GetCurrentDeviceState(outState);
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
