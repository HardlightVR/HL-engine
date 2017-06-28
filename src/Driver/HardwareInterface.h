#pragma once
#include <boost\thread\mutex.hpp>
#include "DriverMessenger.h"
#include "protobuff_defs/EffectCommand.pb.h"
#include "protobuff_defs/HighLevelEvent.pb.h"
#include "Atom.h"
#include "events/BriefTaxel.h"
#include "events_impl/BriefTaxel.h"
#include "events_impl/LastingTaxel.h"
#include "RegionRegistry.h"
class Synchronizer;
class IoService;
class HardwareInterface
{
public:
	HardwareInterface(std::shared_ptr<IoService> io, PluginManager& manager); 
	~HardwareInterface();

	void ReceiveHighLevelEvent(const NullSpaceIPC::HighLevelEvent& event) {
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



	bool m_running;


	PluginManager& m_pluginManager;


	void generateLowLevelSimpleHapticEvents(const NullSpaceIPC::HighLevelEvent& event);
	void generatePlaybackCommands(const NullSpaceIPC::HighLevelEvent& event);
	void generateRealtimeCommands(const NullSpaceIPC::HighLevelEvent& event);
};

