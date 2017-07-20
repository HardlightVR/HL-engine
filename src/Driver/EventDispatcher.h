#pragma once
#include "protobuff_defs/HighLevelEvent.pb.h"
#include <functional>
class EventDispatcher
{
public:

	using EventSelector = std::function<bool(const NullSpaceIPC::HighLevelEvent&)>;
	using EventReceiver = std::function<void(const NullSpaceIPC::HighLevelEvent&)>;

	void InstallFilter(EventSelector selector, EventReceiver receiver);

	void Subscribe(NullSpaceIPC::HighLevelEvent::EventsCase which_event, EventReceiver rec);

	void ReceiveHighLevelEvent(const NullSpaceIPC::HighLevelEvent& event);


		

private:

	struct InstalledFilter {
		EventDispatcher::EventReceiver Receiver;
		EventDispatcher::EventSelector Selector;
	};

	std::vector<InstalledFilter> m_filters;

	std::unordered_map<NullSpaceIPC::HighLevelEvent::EventsCase, std::vector<EventReceiver>> m_subscribers;






	/*void generateLowLevelSimpleHapticEvents(const NullSpaceIPC::HighLevelEvent& event);
	void generatePlaybackCommands(const NullSpaceIPC::HighLevelEvent& event);
	void generateRealtimeCommands(const NullSpaceIPC::HighLevelEvent& event);
	void generateCurve(const NullSpaceIPC::HighLevelEvent& event);*/
};

