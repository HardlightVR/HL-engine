#pragma once
#include "ICommunicationAdapter.h"
#include <boost\thread\mutex.hpp>
#include "FirmwareInterface.h"
#include "ExecutionCommand_generated.h"
#include "DriverMessenger.h"
#include "EffectCommand.pb.h"
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
			_firmware.HaltEffect(Location(ec.area()));
		}
		else if (ec.command() == NullSpaceIPC::EffectCommand_Command_PLAY) {
			_firmware.PlayEffect(Location(ec.area()), ec.effect(), ec.strength());
		}
		else if (ec.command()== NullSpaceIPC::EffectCommand_Command_PLAY_CONTINUOUS) {
			_firmware.PlayEffectContinuous(Location(ec.area()), ec.effect(), ec.strength());
		}
	}

	SuitsConnectionInfo PollDevice();

	void EnableTracking();
	void DisableTracking();
	void RequestSuitVersion();
	void RegisterPacketCallback(SuitPacket::PacketType p, std::function<void(packet p)>);

private:
	PacketDispatcher _dispatcher;

	std::unique_ptr<ICommunicationAdapter> _adapter;
	std::unique_ptr<Synchronizer> _synchronizer;

	boost::asio::deadline_timer _adapterResetCheckTimer;
	boost::posix_time::milliseconds _adapterResetCheckInterval;
	std::thread _adapterResetChecker;

	bool _running;

	boost::condition_variable _needToCheckAdapter;
	boost::mutex _needToCheckMut;

	FirmwareInterface _firmware;

};

