#include "stdafx.h"
#include "HardwareInterface.h"
#include "IoService.h"
#include "PluginManager.h"
#include "Locator.h"
#include "events_impl/PlaybackEvent.h"
#include "events_impl/RealtimeEvent.h"
#include "HardwareCoordinator.h"
#include "cevent_internal.h"
HardwareInterface::HardwareInterface(std::shared_ptr<IoService> ioService, HardwareCoordinator& plugins) :
	m_running(true),
	m_coordinator(plugins)
{
}


HardwareInterface::~HardwareInterface()
{
	m_running = false;
	
}

void HardwareInterface::InstallFilter(EventSelector selector, EventReceiver receiver)
{
	InstalledFilter filter;

	filter.Receiver = receiver;
	filter.Selector = selector;
	m_filters.push_back(std::move(filter));
}


void HardwareInterface::generateLowLevelSimpleHapticEvents(const NullSpaceIPC::HighLevelEvent& event)
{
	const auto& simple_event = event.simple_haptic();
	using namespace nsvr::cevents;

	uint32_t effect = static_cast<uint32_t>(simple_event.effect());

	if (simple_event.duration() == 0.0f) {
		for (const auto& region : simple_event.regions()) {
			m_coordinator.dispatch<BriefHaptic>(
				effect, 
				simple_event.strength(), 
				region.c_str()
			);
		}
	}
	else {
		for (const auto& region : simple_event.regions()) {
			m_coordinator.dispatch<LastingHaptic>(
				effect, 
				simple_event.strength(), 
				simple_event.duration(), 
				region.c_str(),
				event.parent_id()
			);
		}
	}
}

void HardwareInterface::generatePlaybackCommands(const NullSpaceIPC::HighLevelEvent& event)
{
	const auto& playback_event = event.playback_event();
	auto command = static_cast<nsvr_playback_statechange_command>(playback_event.command());
	m_coordinator.dispatch<nsvr::cevents::PlaybackStateChange>(event.parent_id(), command);

}

void HardwareInterface::generateRealtimeCommands(const NullSpaceIPC::HighLevelEvent& event)
{
	const auto& realtime_event = event.realtime_haptic();
	for (const auto& magnitude : realtime_event.magnitudes()) {
	//	nsvr::events::RealtimeEvent realtime;
	//	realtime.Strength = magnitude.strength();
		//m_pluginManager.Dispatch(magnitude.region(), "realtime", AS_TYPE(NSVR_RealtimeEvent, &realtime));
	}
}

void HardwareInterface::generateCurve(const NullSpaceIPC::HighLevelEvent & event)
{
	//m_curveEngine.GenerateCurve(event.parent_id(), event.curve_haptic_event());
}

