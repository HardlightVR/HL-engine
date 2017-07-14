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

struct nsvr_callback {
	void* callback;
	nsvr_core_ctx* context;

	template<typename TCallback, typename...TArgs>
	void call(Args&&...args) {
		static_cast<TCallback>(callback)(context, std::forward<TArgs>(args)...);
	}

};

class HardlightPlugin {
public:
	HardlightPlugin();
	~HardlightPlugin();
	using CallbackTable = std::unordered_map<std::string, nsvr_callback>;

	int Configure(nsvr_core_ctx* ctx);
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

	ImuConsumer m_imus;

	ScheduledEvent m_mockTracking;

	struct core_callbacks {
		CallbackTable callbacks;

		template<typename TCoreCallback, typename...TArgs>
		void call(Args&&...args) {
			auto cb = callbacks.at(typeid(TCoreCallback).name());
			cb.call<TCoreCallback>(std::forward<TArgs>(args)...);
		}
	};

	core_callbacks m_coreApi;

	

};