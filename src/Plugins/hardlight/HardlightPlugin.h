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


struct nsvr_callback {
	void* callback;
	NSVR_Core_Ctx* context;

	template<typename TCoreCallback, typename...Args>
	void call(Args&&...args) {
		TCoreCallback fn = static_cast<TCoreCallback>(callback);
		(fn)(context, std::forward<Args>(args)...);
	}

};

class HardlightPlugin {
public:
	HardlightPlugin();
	~HardlightPlugin();
	int Configure(NSVR_Configuration* config);
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

	bool m_running;

	
	std::unordered_map<std::string, nsvr_callback> m_coreApi;


};