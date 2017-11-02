#include "stdafx.h"
#include "DeviceManager.h"
#include <iostream>
#include "HardlightPlugin.h"

static std::array<uint8_t, 16> version_packet = { 0x24,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x0D,0x0A };
static std::array<uint8_t, 16> uuid_packet = { 0x24,0x02,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x0D,0x0A };

void requestSuitVersion(std::unique_ptr<BoostSerialAdapter>& adapter)
{
	adapter->Write(version_packet.data(), version_packet.size(), [](auto ec, std::size_t) { if (ec) { std::cout << "Error writing suit version\n"; } });
}

void requestUuid(std::unique_ptr<BoostSerialAdapter>& adapter)
{
	adapter->Write(version_packet.data(), version_packet.size(), [](auto ec, std::size_t) { if (ec) { std::cout << "Error writing uuid packet\n"; } });
}

DeviceManager::DeviceManager(std::string path)
	: m_ioService()
	, m_path(path)
	, m_deviceIds()
	, m_recognizer(m_ioService.GetIOService())
	, m_requestVersionTimeout(boost::posix_time::millisec(200))
	, m_requestVersionTimer(m_ioService.GetIOService())
	, m_idPool()
	, m_devicePollTimeout(boost::posix_time::millisec(5))
	, m_devicePollTimer(m_ioService.GetIOService())
{
	

	m_recognizer.on_recognize([this](connection_info info) {

		
		auto potentialDevice = std::make_unique<PotentialDevice>(m_ioService.GetIOService());
		potentialDevice->adapter->Connect(info);
		potentialDevice->synchronizer->start();
		potentialDevice->dispatcher->AddConsumer(PacketType::SuitVersion, [this, port = info.port](auto packet) {
			handle_connect(port, packet);
		});
		//makes the lifetime requirements clear: synchronizer must not outlive the dispatcher
		potentialDevice->synchronizer->on_packet([dispatcher = potentialDevice->dispatcher.get()](auto packet) {
			dispatcher->Dispatch(std::move(packet)); 
		});
		requestSuitVersion(potentialDevice->adapter);
		requestSuitVersion(potentialDevice->adapter);
		requestUuid(potentialDevice->adapter);

		m_potentials.insert(std::make_pair(info.port, std::move(potentialDevice)));
	});

	m_recognizer.on_unrecognize([this](connection_info info) {

		auto it = std::find_if(m_deviceIds.begin(), m_deviceIds.end(), [port = info.port](const auto& kvp) { return kvp.second == port; });

		if (it != m_deviceIds.end()) {
			nsvr_device_event_raise(m_core, nsvr_device_event_device_disconnected, it->first);
			m_idPool.Release(it->first);

			m_devices.erase(it->second);
			m_deviceIds.erase(it);
		}



	
	});
	m_recognizer.start();


	m_devicePollTimer.expires_from_now(m_devicePollTimeout);
	m_devicePollTimer.async_wait([this](auto ec) { if (ec) { return; } 
		device_update();
	});
}

void DeviceManager::handle_connect(std::string portName, Packet packet) {

	if (m_potentials.find(portName) == m_potentials.end()) {
		return;
	}

	/*
		//if suit version is mark2, then create 
	
	*/
	auto potential = std::move(m_potentials.at(portName));

	m_potentials.erase(portName);

	potential->dispatcher->ClearConsumers();
	
	auto real = std::make_unique<HardlightPlugin>(m_ioService.GetIOService(), m_path, std::move(potential));
	real->Configure(m_core);

	m_devices.insert(std::make_pair(portName, std::move(real)));

	auto id = m_idPool.Request();
	m_deviceIds[id] = portName;

	nsvr_device_event_raise(m_core, nsvr_device_event_device_connected, id);


}

void DeviceManager::device_update()
{
	for (auto& kvp : m_devices) {
		kvp.second->PollEvents();
	}

	m_devicePollTimer.expires_from_now(m_devicePollTimeout);
	m_devicePollTimer.async_wait([this](auto ec) { if (ec) { return; }
		device_update();
	});
}

void DeviceManager::send_version_requests()
{
	for (auto& kvp : m_potentials) {
		requestSuitVersion(kvp.second->adapter);
	}

	m_requestVersionTimer.expires_from_now(m_requestVersionTimeout);
	m_requestVersionTimer.async_wait([this](auto ec) { if (ec) { return; } send_version_requests(); });

}

DeviceManager::~DeviceManager()
{
	m_recognizer.stop();
	m_devicePollTimer.cancel();
}

int DeviceManager::configure(nsvr_core * core)
{
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

	return 1;
}

void DeviceManager::Render(nsvr_diagnostics_ui* ui) {
	for (auto& device : m_devices) {
		device.second->Render(ui);
	}
}
void DeviceManager::EnumerateNodesForDevice(nsvr_device_id id, nsvr_node_ids * ids)
{
	auto portName = m_deviceIds.at(id);
	m_devices.at(portName)->EnumerateNodesForDevice(ids);
}

void DeviceManager::EnumerateDevices(nsvr_device_ids * ids)
{
	ids->device_count = m_deviceIds.size();

	std::size_t index = 0;
	for (const auto& id : m_deviceIds) {
		ids->ids[index++] = id.first;
	}
}

void DeviceManager::GetDeviceInfo(nsvr_device_id id, nsvr_device_info * info)
{

	auto portName = m_deviceIds.at(id);
	m_devices.at(portName)->GetDeviceInfo(info);
}

void DeviceManager::GetNodeInfo(nsvr_device_id device_id, nsvr_node_id node_id, nsvr_node_info * info)
{
	auto portName = m_deviceIds.at(device_id);
	m_devices.at(portName)->GetNodeInfo(node_id, info);
}
