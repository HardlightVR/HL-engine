#include "stdafx.h"
#include <functional>
#include "SuitVersionInfo.h"
#include "runtime_include/NSDriverApi.h"
#include "logger.h"


#include "Driver.h"
#include "Enums.h"
#include "IoService.h"
#include "DeviceBuilder.h"


constexpr int MIN_CLIENT_VERSION_MAJOR = 1;
constexpr int MIN_CLIENT_VERSION_MINOR = 0;
constexpr int MIN_CLIENT_VERSION_PATCH = 0;
bool isAtLeastVersion(int major, int minor, int patch)
{
	if (MIN_CLIENT_VERSION_MAJOR > major) return false;
	if (MIN_CLIENT_VERSION_MAJOR < major) return true;
	if (MIN_CLIENT_VERSION_MINOR > minor) return false;
	if (MIN_CLIENT_VERSION_MINOR < minor) return true;
	if (MIN_CLIENT_VERSION_PATCH > patch) return false;
	return true;
}
Driver::Driver() :
	m_ioService(),
	m_io(m_ioService.GetIOService()),
	m_messenger(m_io),

	m_pluginManager(m_io),
	m_devices(),
	m_coordinator(m_io, m_messenger, m_devices),

	m_statusPush(m_io, boost::posix_time::millisec(250)),
	m_hapticsPull(m_io, boost::posix_time::millisec(5)),
	m_cachedTracking({}),
	m_eventDispatcher(),
	m_renderingApi(),
	m_currentSuitState(0)



{
	//BOOST_LOG_SEV(clogger::get(), nsvr_loglevel_info) << "Plugin host initialized";
	LOG_INFO() << "Plugin host initialized";

	m_pluginManager.OnFatalError([this]() {
		this->Shutdown();
		std::exit(-100);
	});
	m_pluginManager.SetDeviceContainer(&m_devices);

	m_pluginManager.Discover();
	m_pluginManager.LoadAll();

	m_coordinator.SetupSubscriptions(m_eventDispatcher);


	m_eventDispatcher.Subscribe(NullSpaceIPC::HighLevelEvent::EventsCase::kClientIdEvent, [](const auto& event) {
		auto ce = event.client_id_event();
		LOG_INFO() << "Client connected; Hardlight.dll version " << ce.dll_major() << "." << ce.dll_minor() << "." << ce.dll_patch();
		if (!isAtLeastVersion(ce.dll_major(), ce.dll_minor(), ce.dll_patch())) {
			LOG_WARN() << "--> Client is PROBABLY NOT COMPATIBLE with this runtime";
		}
	
	});


	
}



bool Driver::StartThread()
{
	

	m_hapticsPull.SetEvent([this]() { handleHaptics(); });
	m_hapticsPull.Start();

	m_statusPush.SetEvent([this]() { handleStatus(); });
	m_statusPush.Start();

	

	return true;
}

bool Driver::Shutdown()
{
	BOOST_LOG_SEV(clogger::get(), nsvr_severity_info) << "Shutting down plugin host";

	m_statusPush.Stop();
	m_hapticsPull.Stop();
	m_messenger.Disconnect();
	m_ioService.Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return true;
}

void Driver::DrawDiagnostics(uint32_t id)
{
	nsvr_diagnostics_ui ui;
	ui.button = m_renderingApi.button;
	ui.keyval = m_renderingApi.keyval;
	ui.slider_int = m_renderingApi.slider_int;
	m_pluginManager.DrawDiagnostics(id, &ui);

}

int Driver::EnumeratePlugins(hvr_plugin_list * outPlugins)
{
	auto ids = m_pluginManager.GetPluginIds();
	std::copy(ids.begin(), ids.end(), outPlugins->ids);
	outPlugins->count = ids.size();
	return 1;
}


int Driver::GetPluginInfo(hvr_plugin_id id, hvr_plugin_info* outInfo) {
	if (auto optionalInfo = m_pluginManager.GetPluginInfo(id)) {
		std::copy(std::begin(optionalInfo->first), std::end(optionalInfo->first), outInfo->name);
		return 1;
	}
	else {
		return 0;
	}
}

void Driver::GetCurrentSuitState(int * outStatus, uint64_t* outError)
{
	
	
	m_pluginManager.GetHardlightPlugin()->apis().GetApi<verification_api>()->submit_getcurrentdevicestate(outStatus, outError);

}



void Driver::handleHaptics()
{
	//note: changed to ReadEvents instead of ReadHaptics
	if (auto commands = m_messenger.ReadEvents()) {
		for (const auto& command : *commands) {
			m_eventDispatcher.ReceiveHighLevelEvent(command);
		}
	}

	
}

void Driver::handleStatus()
{
	//m_messenger.WriteSystems(m_hardware.PollDevice());
}

void DoForEachBit(std::function<void(Location l)> fn, uint32_t bits) {
	for (uint32_t bit = 1; bits >= bit; bit *= 2) if (bits & bit) fn(Location(bit));

}


void Driver::ProvideRenderingApi(hvr_diagnostics_ui * api)
{
	m_renderingApi = *api;
}

