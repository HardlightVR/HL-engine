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
	_running(true)
{


	_adapter->Connect();
	

}


HardwareInterface::~HardwareInterface()
{
	_running = false;
	if (_adapterResetChecker.joinable()) {
		_adapterResetChecker.join();
	}
}

