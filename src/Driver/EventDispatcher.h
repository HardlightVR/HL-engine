#pragma once
#include "protobuff_defs/HighLevelEvent.pb.h"
#include <functional>
#include <boost/signals2.hpp>
class EventDispatcher
{
public:

	using EventSelector = std::function<bool(const NullSpaceIPC::HighLevelEvent&)>;
	using EventReceiver = std::function<void(const NullSpaceIPC::HighLevelEvent&)>;
	using EventSignal = boost::signals2::signal<void(const NullSpaceIPC::HighLevelEvent&)>;


	void Subscribe(NullSpaceIPC::HighLevelEvent::EventsCase which_event,  EventSignal::slot_type rec);
	void Subscribe(std::initializer_list<NullSpaceIPC::HighLevelEvent::EventsCase> events, EventSignal::slot_type rec);
	void ReceiveHighLevelEvent(const NullSpaceIPC::HighLevelEvent& event);


		

private:

	struct InstalledFilter {
		EventDispatcher::EventReceiver Receiver;
		EventDispatcher::EventSelector Selector;
	};

	std::vector<InstalledFilter> m_filters;

	std::unordered_map<NullSpaceIPC::HighLevelEvent::EventsCase, EventSignal> m_subscribers;






	/*void generateLowLevelSimpleHapticEvents(const NullSpaceIPC::HighLevelEvent& event);
	void generatePlaybackCommands(const NullSpaceIPC::HighLevelEvent& event);
	void generateRealtimeCommands(const NullSpaceIPC::HighLevelEvent& event);
	void generateCurve(const NullSpaceIPC::HighLevelEvent& event);*/
};

