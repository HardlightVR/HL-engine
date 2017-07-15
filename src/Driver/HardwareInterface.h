#pragma once
#include <boost\thread\mutex.hpp>
#include "DriverMessenger.h"
#include "protobuff_defs/EffectCommand.pb.h"
#include "protobuff_defs/HighLevelEvent.pb.h"

#include "Enums.h"
class Synchronizer;
class IoService;
class HardwareCoordinator;
class HardwareInterface
{
public:
	HardwareInterface(std::shared_ptr<IoService> io,HardwareCoordinator& manager); 
	~HardwareInterface();

	using EventSelector = std::function<bool(const NullSpaceIPC::HighLevelEvent&)>;
	using EventReceiver = std::function<void(const NullSpaceIPC::HighLevelEvent&)>;

	void InstallFilter(EventSelector selector, EventReceiver receiver);

	void ReceiveHighLevelEvent(const NullSpaceIPC::HighLevelEvent& event) {

		for (const auto& filter : m_filters) {
			if (filter.Selector(event)) { filter.Receiver(event); }
		}


		switch (event.events_case()) {
		case NullSpaceIPC::HighLevelEvent::kSimpleHaptic:
			generateLowLevelSimpleHapticEvents(event);
			break;
		case NullSpaceIPC::HighLevelEvent::kPlaybackEvent:
			generatePlaybackCommands(event);
			break;
		case NullSpaceIPC::HighLevelEvent::kRealtimeHaptic:
			generateRealtimeCommands(event);
			break;
		case NullSpaceIPC::HighLevelEvent::kCurveHaptic:
			generateCurve(event);
		case NullSpaceIPC::HighLevelEvent::EVENTS_NOT_SET:
			//fall-through
		default:
			break;
		}
	}


	SuitsConnectionInfo PollDevice();
	void ReadDriverData(Location loc);
	void EnableTracking();
	void DisableTracking();


//	void EnableAudioMode(Location pad, const FirmwareInterface::AudioOptions& options);
	void EnableIntrigMode(Location pad);
	void EnableRtpMode(Location pad);

	void RequestSuitVersion();
//	void RegisterPacketCallback(SuitPacket::PacketType p, std::function<void(packet p)>);

	void RawCommand(const std::string& command);

private:

	struct InstalledFilter {
		HardwareInterface::EventReceiver Receiver;
		HardwareInterface::EventSelector Selector;
	};

	std::vector<InstalledFilter> m_filters;


	bool m_running;


	HardwareCoordinator& m_coordinator;


	void generateLowLevelSimpleHapticEvents(const NullSpaceIPC::HighLevelEvent& event);
	void generatePlaybackCommands(const NullSpaceIPC::HighLevelEvent& event);
	void generateRealtimeCommands(const NullSpaceIPC::HighLevelEvent& event);
	void generateCurve(const NullSpaceIPC::HighLevelEvent& event);
};

