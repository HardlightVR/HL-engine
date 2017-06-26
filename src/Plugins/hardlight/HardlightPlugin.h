#pragma once
#include "PluginAPI.h"
#include <stdint.h>
#include "PacketDispatcher.h"
#include <memory>
#include "KeepaliveMonitor.h"
#include "BoostSerialAdapter.h"
#include "Synchronizer.h"
#include <thread>
#include "FirmwareInterface.h"
#include "IoService.h"
class HardlightPlugin {
public:
	HardlightPlugin();
	~HardlightPlugin();
	int Configure(NSVR_Core* core);
	int PlayBrief(Location loc, uint32_t effect, float strength);
private:
	std::shared_ptr<IoService> m_io;
	PacketDispatcher m_dispatcher;
	std::unique_ptr<BoostSerialAdapter> m_adapter;
	FirmwareInterface m_firmware;

	std::shared_ptr<KeepaliveMonitor> m_monitor;

	std::unique_ptr<Synchronizer> m_synchronizer;



	bool m_running;

	

public:
	void PlayLasting(uint64_t id, Location location, uint32_t effect, float strength);
};