#pragma once
#include "ICommunicationAdapter.h"
#include <boost\thread\mutex.hpp>
#include "FirmwareInterface.h"
#include "DriverMessenger.h"
#include "protobuff_defs/EffectCommand.pb.h"
#include "protobuff_defs/HighLevelEvent.pb.h"
#include "Atom.h"
#include "KeepaliveMonitor.h"
#include "PacketDispatcher.h"
#include "events/BriefTaxel.h"
#include "events_impl/BriefTaxel.h"
#include "events_impl/LastingTaxel.h"
#include "RegionRegistry.h"
class Synchronizer;
class IoService;
class HardwareInterface
{
public:
	HardwareInterface(std::shared_ptr<IoService> io, RegionRegistry& registry); //(io, regionRegistry)
	~HardwareInterface();

	void ReceiveHighLevelEvent(const NullSpaceIPC::HighLevelEvent& event) {
		switch (event.events_case()) {
		case NullSpaceIPC::HighLevelEvent::kSimpleHaptic:
			generateLowLevelSimpleHapticEvents(event);
			break;
		case NullSpaceIPC::HighLevelEvent::EVENTS_NOT_SET:
			//fall-through
		default:
			break;
		}
	}
	void ReceiveExecutionCommand(const NullSpaceIPC::EffectCommand& ec) {
		


		if (ec.command() == NullSpaceIPC::EffectCommand_Command_HALT) {
			
			m_firmware.HaltEffect(Location(ec.area()));
		}
		else if (ec.command() == NullSpaceIPC::EffectCommand_Command_PLAY) {
			nsvr::events::BriefTaxel taxel = { 0 };
			taxel.Effect = ec.effect();
			taxel.Strength = ec.strength();

			std::string locString = Locator::Translator().ToString(Location(ec.area()));
			m_registry.Activate(locString, "brief-taxel", AS_TYPE(NSVR_BriefTaxel, &taxel));

			//m_firmware.PlayEffect(Location(ec.area()), ec.effect(), ec.strength());
		}
		else if (ec.command()== NullSpaceIPC::EffectCommand_Command_PLAY_CONTINUOUS) {
			m_firmware.PlayEffectContinuous(Location(ec.area()), ec.effect(), ec.strength());
		}
		else if (ec.command() == NullSpaceIPC::EffectCommand_Command_PLAY_RTP) {
			int strength = int(ec.strength() * 128.0f);
			m_firmware.PlayRtp(Location(ec.area()), strength);
		}
		else if (ec.command() == NullSpaceIPC::EffectCommand_Command_ENABLE_RTP) {
			m_firmware.EnableRtpMode(Location(ec.area()));
		}
		else if (ec.command() == NullSpaceIPC::EffectCommand_Command_ENABLE_INTRIG) {
			m_firmware.EnableIntrigMode(Location(ec.area()));
		}
	}

	SuitsConnectionInfo PollDevice();
	void ResetDrivers();
	void ReadDriverData(Location loc);
	void EnableTracking();
	void DisableTracking();


	void EnableAudioMode(Location pad, const FirmwareInterface::AudioOptions& options);
	void EnableIntrigMode(Location pad);
	void EnableRtpMode(Location pad);

	void RequestSuitVersion();
	void RegisterPacketCallback(SuitPacket::PacketType p, std::function<void(packet p)>);

	void RawCommand(const std::string& command);

private:
	PacketDispatcher m_dispatcher;
	std::unique_ptr<ICommunicationAdapter> m_adapter;
	std::shared_ptr<KeepaliveMonitor> m_monitor;
	FirmwareInterface m_firmware;

	std::unique_ptr<Synchronizer> m_synchronizer;

	std::thread m_adapterResetChecker;

	bool m_running;


	RegionRegistry& m_registry;


	void generateLowLevelSimpleHapticEvents(const NullSpaceIPC::HighLevelEvent& event);
};

