#pragma once
#include <cstdint>
#include <memory>

#include "PluginAPI.h"
#include "PacketDispatcher.h"
#include "FirmwareInterface.h"
#include "ImuConsumer.h"
#include "zone_logic/hardlightdevice.h"
#include "ScheduledEvent.h"

class synchronizer2;
struct PotentialDevice;
class BoostSerialAdapter;
class Heartbeat;
class Synchronizer;

class HardlightPlugin {
public:
	HardlightPlugin(boost::asio::io_service& io, const std::string& data_dir, std::unique_ptr<PotentialDevice> device);
	~HardlightPlugin();

	int Configure(nsvr_core* ctx);
	void BeginTracking(nsvr_tracking_stream* stream, nsvr_node_id region);
	void EndTracking(nsvr_node_id region);
	void EnumerateNodesForDevice(nsvr_node_ids* ids);
	void EnumerateDevices(nsvr_device_ids* ids);
	void GetDeviceInfo(nsvr_device_info* info);
	void GetNodeInfo(nsvr_node_id id, nsvr_node_info* info);
	void SetupBodygraph(nsvr_bodygraph* graph);

	void Render(nsvr_diagnostics_ui* ui);

	void PollEvents();
private:
	nsvr_core* m_core;


	boost::asio::io_service& m_io;

	std::unique_ptr<PacketDispatcher> m_dispatcher;

	std::shared_ptr<FirmwareInterface> m_firmware;

	HardlightDevice m_device;
	std::shared_ptr<Heartbeat> m_monitor;

	std::shared_ptr<synchronizer2> m_synchronizer;

	bool m_running;

	ImuConsumer m_imus;



	

};