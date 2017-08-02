#include "stdafx.h"
#include "EventDispatcher.h"
#include "IoService.h"
#include "PluginManager.h"
#include "Locator.h"

#include "cevent_internal.h"
#include "nsvr_playback_handle.h"



#include <boost/log/trivial.hpp>



void EventDispatcher::Subscribe(NullSpaceIPC::HighLevelEvent::EventsCase which_event, EventSignal::slot_type rec)
{
	m_subscribers[which_event].connect(rec);
}



void EventDispatcher::ReceiveHighLevelEvent(const NullSpaceIPC::HighLevelEvent& event)
{
	auto item = m_subscribers.find(event.events_case());
	if (item != m_subscribers.end()) {
		(item->second)(event);
	}
	else {
		BOOST_LOG_TRIVIAL(warning) << "[EventDispatcher] Unrecognized event type: " << event.events_case();
	}
}

