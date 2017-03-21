#include "stdafx.h"
#include "HardwareInterface.h"
#include "BoostSerialAdapter.h"
#include "IoService.h"
#include "Locator.h"
HardwareInterface::HardwareInterface(std::shared_ptr<IoService> ioService) :
	
	m_adapter(std::make_unique<BoostSerialAdapter>(ioService->GetIOService())),
	m_synchronizer(std::make_unique<Synchronizer>(m_adapter->GetDataStream(), m_dispatcher, ioService->GetIOService())),
	m_adapterResetCheckTimer(ioService->GetIOService()),
	m_adapterResetCheckInterval(boost::posix_time::milliseconds(50)),
	m_running(true),
	m_firmware(m_adapter, ioService->GetIOService())

{
	m_adapter->Connect();
	m_synchronizer->BeginSync();
}


HardwareInterface::~HardwareInterface()
{
	m_running = false;
	if (m_adapterResetChecker.joinable()) {
		m_adapterResetChecker.join();
	}
}

SuitsConnectionInfo HardwareInterface::PollDevice()
{
	SuitsConnectionInfo info = {};
	info.timestamp = std::time(nullptr);
	info.SuitsFound[0] = true;
	info.Suits[0].Id = 1;
	info.Suits[0].Status = m_adapter->IsConnected() ? 
		NullSpace::SharedMemory::Connected : 
		NullSpace::SharedMemory::Disconnected;

	return info;
}

void HardwareInterface::EnableTracking()
{
	m_firmware.RequestSuitVersion();
	m_firmware.EnableTracking();
	
}

void HardwareInterface::DisableTracking()
{
	m_firmware.DisableTracking();
}

void HardwareInterface::RequestSuitVersion()
{
	m_firmware.RequestSuitVersion();
}

void HardwareInterface::RegisterPacketCallback(SuitPacket::PacketType p, std::function<void(packet p)> func)
{
	m_dispatcher.AddConsumer(p, func);
}

