#include "stdafx.h"
#include "HardwareInterface.h"
#include "BoostSerialAdapter.h"
#include "IoService.h"
#include "Locator.h"
HardwareInterface::HardwareInterface(std::shared_ptr<IoService> ioService) :
	_adapter(std::make_unique<BoostSerialAdapter>(ioService)),
	_synchronizer(std::make_unique<Synchronizer>(_adapter->GetDataStream(), nullptr, ioService->GetIOService())),
	_adapterResetCheckTimer(ioService->GetIOService()),
	_adapterResetCheckInterval(boost::posix_time::milliseconds(50)),
	_running(true),
	_firmware(_adapter, ioService->GetIOService())
{


	_adapter->Connect();
	_synchronizer->BeginSync();

}


HardwareInterface::~HardwareInterface()
{
	_running = false;
	if (_adapterResetChecker.joinable()) {
		_adapterResetChecker.join();
	}
}

SuitsConnectionInfo HardwareInterface::PollDevices()
{
	SuitsConnectionInfo info = {};
	info.timestamp = std::time(nullptr);
	info.SuitsFound[0] = true;
	info.Suits[0].Id = 1;
	info.Suits[0].Status = _adapter->IsConnected() ? 
		NullSpace::SharedMemory::Connected : 
		NullSpace::SharedMemory::Disconnected;

	return info;
}

