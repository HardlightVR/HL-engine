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
#include "zone_logic/hardlightdevice.h"
#include "ScheduledEvent.h"
#include <functional>
#include "ImuConsumer.h"



class HardlightPlugin {
public:
	HardlightPlugin();
	~HardlightPlugin();

	int Configure(nsvr_core* ctx);
private:
	std::shared_ptr<IoService> m_io;
	PacketDispatcher m_dispatcher;
	std::unique_ptr<BoostSerialAdapter> m_adapter;
	FirmwareInterface m_firmware;

	//just give out the shared pointers address
	HardlightDevice m_device;
	std::shared_ptr<KeepaliveMonitor> m_monitor;

	std::unique_ptr<Synchronizer> m_synchronizer;

	ScheduledEvent m_eventPull;
	nsvr_core* m_core;

	bool m_running;

	ImuConsumer m_imus;

	ScheduledEvent m_mockTracking;
	nsvr_querystate* m_querystate;


	

};