#include "stdafx.h"
#include "Driver.h"
#include "IoService.h"
#include "ClientMessenger.h" 
#include "IntermediateHapticFormats.h"
#include "Encoder.h"
Driver::Driver() :
	_io(new IoService()),
	m_running(false),
	m_hapticsPollTimer(_io->GetIOService()),
	m_hapticsPollInterval(5),
	m_hardware(_io),
	m_messenger(_io->GetIOService()),
	m_dispatcher(),
	m_statusPushTimer(_io->GetIOService()),
	m_statusPushInterval(250)

{
}

Driver::~Driver()
{
	if (_workThread.joinable()) {
		_workThread.join();
	}
}

bool Driver::StartThread()
{
	m_running = true;
	scheduleHapticsPoll();
	scheduleStatusPush();
	

	return true;
}

bool Driver::Shutdown()
{
	m_running.store(false);
	m_hapticsPollTimer.cancel();
	m_statusPushTimer.cancel();
	
	m_messenger.Disconnect();
	
	_io->Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return true;
}

void Driver::handleHaptics(const boost::system::error_code& ec)
{
	if (!ec) {
		if (auto commands = m_messenger.ReadHaptics()) {
			for (const auto& command : *commands) {
				m_hardware.ReceiveExecutionCommand(command);
			}
		}
		scheduleHapticsPoll();
	}


}

void Driver::handleStatus(const boost::system::error_code &ec)
{
	if (!ec) {
		m_messenger.WriteSuits(m_hardware.PollDevices());
		scheduleStatusPush();
	}
	
}

void Driver::scheduleHapticsPoll()
{
	m_hapticsPollTimer.expires_from_now(m_hapticsPollInterval);
	m_hapticsPollTimer.async_wait(boost::bind(&Driver::handleHaptics, this, boost::asio::placeholders::error));
}

void Driver::scheduleStatusPush()
{
	m_statusPushTimer.expires_from_now(m_statusPushInterval);
	m_statusPushTimer.async_wait(boost::bind(&Driver::handleStatus, this, boost::asio::placeholders::error));
}

