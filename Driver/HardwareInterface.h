#pragma once
#include "ICommunicationAdapter.h"
#include <boost\thread\mutex.hpp>
#include "FirmwareInterface.h"
#include "ExecutionCommand_generated.h"
#include "DriverMessenger.h"
class Synchronizer;
class IoService;
class HardwareInterface
{
public:
	HardwareInterface(std::shared_ptr<IoService> io);
	~HardwareInterface();
	void ReceiveExecutionCommand(const ExecutionCommand& ec) {
		if (ec.Command == NullSpace::HapticFiles::PlayCommand_HALT) {
			_firmware.HaltEffect(Location(ec.Location));
		}
		else if (ec.Command == NullSpace::HapticFiles::PlayCommand_PLAY) {
			_firmware.PlayEffect(Location(ec.Location), Effect(ec.Effect));
		}
		else if (ec.Command == NullSpace::HapticFiles::PlayCommand_PLAY_CONTINUOUS) {
			_firmware.PlayEffectContinuous(Location(ec.Location), Effect(ec.Effect));
		}
	}

private:
	std::unique_ptr<ICommunicationAdapter> _adapter;
	std::unique_ptr<Synchronizer> _synchronizer;

	boost::asio::deadline_timer _adapterResetCheckTimer;
	boost::posix_time::milliseconds _adapterResetCheckInterval;

	std::thread _adapterResetChecker;

	bool _running;

	boost::condition_variable _needToCheckAdapter;
	boost::mutex _needToCheckMut;
	bool _dataReady;

	FirmwareInterface _firmware;

};

