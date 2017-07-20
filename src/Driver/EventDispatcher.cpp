#include "stdafx.h"
#include "EventDispatcher.h"
#include "IoService.h"
#include "PluginManager.h"
#include "Locator.h"

#include "cevent_internal.h"
#include "nsvr_playback_handle.h"





void EventDispatcher::InstallFilter(EventSelector selector, EventReceiver receiver)
{
	
		InstalledFilter filter;

		filter.Receiver = receiver;
		filter.Selector = selector;
		m_filters.push_back(std::move(filter));
	

}

void EventDispatcher::Subscribe(NullSpaceIPC::HighLevelEvent::EventsCase which_event, EventReceiver rec)
{


		m_subscribers[which_event].push_back(rec);
	
}

void EventDispatcher::ReceiveHighLevelEvent(const NullSpaceIPC::HighLevelEvent& event)
{
	for (auto& subscriber : m_subscribers[event.events_case()]) {
		subscriber(event);
	}
}

