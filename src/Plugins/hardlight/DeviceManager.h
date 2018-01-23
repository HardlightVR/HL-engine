#pragma once

#include "serial/hardware_device_recognizer.h"

#include "PacketDispatcher.h"
#include "IoService.h"
#include "Heartbeat.h"

#include "synchronizer2.h"
#include "HardwareIO.h"
#include "IdPool.h"
#include "Doctor.h"

#include <mutex>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class Device;

struct PotentialDevice {
	std::unique_ptr<HardwareIO> io;
	std::shared_ptr<PacketDispatcher> dispatcher;
	std::shared_ptr<synchronizer2> synchronizer;

	PotentialDevice(std::unique_ptr<boost::asio::serial_port> port) {
		boost::asio::io_service& io_service = port->get_io_service();
		io = std::make_unique<HardwareIO>(std::move(port));
		dispatcher = std::make_shared<PacketDispatcher>();
		synchronizer = std::make_shared<synchronizer2>(io_service, io->incoming_queue());
	}
};

//hack enum for hardware verification app 
enum class SuitState {
	Unknown = 0,
	Unplugged = 1,
	Checking = 2,
	Ok = 3,
	Error = 4
};
class DeviceManager {
public:
	DeviceManager(std::string path);
	~DeviceManager();

	int configure(nsvr_core* core);

	void EnumerateNodesForDevice(nsvr_device_id, nsvr_node_ids* ids);
	void EnumerateDevices(nsvr_device_ids* ids);
	void GetDeviceInfo(nsvr_device_id id, nsvr_device_info* info);
	void GetNodeInfo(nsvr_device_id device_id, nsvr_node_id id, nsvr_node_info* info);
	void Render(nsvr_diagnostics_ui* ui);


	//hack for hardware verification app
	void GetCurrentDeviceState(int* status, uint64_t* error);
private:
	
	void handle_connect(std::string portName, Packet packet);
	void device_update();
	void handle_recognize(connection_info info);
	void handle_unrecognize(connection_info info);

	nsvr_core* m_core;
	IoService m_ioService;
	std::string m_path;
	
	Doctor m_doctor;

	hardware_device_recognizer m_recognizer;

	std::unordered_map<std::size_t, std::string> m_deviceIds;
	std::unordered_map<std::string, std::unique_ptr<Device>> m_devices;
	std::unordered_map<std::string, std::unique_ptr<PotentialDevice>> m_potentials;

	boost::posix_time::milliseconds m_requestVersionTimeout;
	boost::asio::deadline_timer m_requestVersionTimer;

	boost::posix_time::millisec m_devicePollTimeout;
	boost::asio::deadline_timer m_devicePollTimer;

	IdPool m_idPool;

	std::mutex m_deviceLock;

};