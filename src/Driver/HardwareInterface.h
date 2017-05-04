#pragma once
#include "ICommunicationAdapter.h"
#include <boost\thread\mutex.hpp>
#include "FirmwareInterface.h"
#include "DriverMessenger.h"
#include "protobuff_defs/EffectCommand.pb.h"
#include "Atom.h"
#include "PacketDispatcher.h"
class Synchronizer;
class IoService;
class HardwareInterface
{
public:
	HardwareInterface(std::shared_ptr<IoService> io);
	~HardwareInterface();
	//these all will eventually either be parameterized over each device, or take a device id
	void ReceiveExecutionCommand(const NullSpaceIPC::EffectCommand& ec) {
		if (ec.command() == NullSpaceIPC::EffectCommand_Command_HALT) {
			m_firmware.HaltEffect(Location(ec.area()));
		}
		else if (ec.command() == NullSpaceIPC::EffectCommand_Command_PLAY) {
			m_firmware.PlayEffect(Location(ec.area()), ec.effect(), ec.strength());
		}
		else if (ec.command()== NullSpaceIPC::EffectCommand_Command_PLAY_CONTINUOUS) {
			m_firmware.PlayEffectContinuous(Location(ec.area()), ec.effect(), ec.strength());
		}
		else if (ec.command() == NullSpaceIPC::EffectCommand_Command_PLAY_RTP) {
			int strength = int(ec.strength() * 255.0f);

			m_firmware.PlayRtp(Location(ec.area()), strength);
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
	std::unique_ptr<Synchronizer> m_synchronizer;

	boost::asio::deadline_timer m_adapterResetCheckTimer;
	boost::posix_time::milliseconds m_adapterResetCheckInterval;
	std::thread m_adapterResetChecker;

	bool m_running;

	boost::condition_variable _needToCheckAdapter;
	boost::mutex _needToCheckMut;

	FirmwareInterface m_firmware;

};
