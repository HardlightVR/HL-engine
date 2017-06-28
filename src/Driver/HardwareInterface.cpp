#include "stdafx.h"
#include "HardwareInterface.h"
#include "IoService.h"
#include "PluginManager.h"
#include "Locator.h"
#include "events_impl/PlaybackEvent.h"
#include "events_impl/RealtimeEvent.h"

HardwareInterface::HardwareInterface(std::shared_ptr<IoService> ioService, PluginManager& plugins) :


	
	m_running(true),
	m_pluginManager(plugins)
	

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

	if (simple_event.duration() == 0.0f) {
		nsvr::events::BriefTaxel taxel = { 0 };
		taxel.Effect = simple_event.effect();
		taxel.Strength = simple_event.strength();
		for (const auto& region : simple_event.regions()) {
			m_pluginManager.Dispatch(region, "brief-taxel", AS_TYPE(NSVR_BriefTaxel, &taxel));
		}
	}
	else {
		nsvr::events::LastingTaxel taxel = { 0 };
		taxel.Effect = simple_event.effect();
		taxel.Strength = simple_event.strength();
		taxel.Duration = simple_event.duration();
		taxel.Id = event.parent_id();
		for (const auto& region : simple_event.regions()) {
			m_pluginManager.Dispatch(region, "lasting-taxel", AS_TYPE(NSVR_LastingTaxel, &taxel));
		}
	}
}

void HardwareInterface::generatePlaybackCommands(const NullSpaceIPC::HighLevelEvent& event)
{
	const auto& playback_event = event.playback_event();
	nsvr::events::PlaybackEvent playback;
	playback.Command = static_cast<NSVR_PlaybackEvent_Command>(playback_event.command());
	playback.Id = event.parent_id();
	m_pluginManager.Broadcast("playback-controls", AS_TYPE(NSVR_PlaybackEvent, &playback));

}

void HardwareInterface::generateRealtimeCommands(const NullSpaceIPC::HighLevelEvent& event)
{
	const auto& realtime_event = event.realtime_haptic();
	for (const auto& magnitude : realtime_event.magnitudes()) {
		nsvr::events::RealtimeEvent realtime;
		realtime.Strength = magnitude.strength();
		m_pluginManager.Dispatch(magnitude.region(), "realtime", AS_TYPE(NSVR_RealtimeEvent, &realtime));
	}
}

void HardwareInterface::generateCurve(const NullSpaceIPC::HighLevelEvent & event)
{
	//m_curveEngine.GenerateCurve(event.parent_id(), event.curve_haptic_event());
}

