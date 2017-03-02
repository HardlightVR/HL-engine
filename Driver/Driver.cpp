#include "stdafx.h"
#include "Driver.h"
#include "IoService.h"
#include "ClientMessenger.h" 
#include "IntermediateHapticFormats.h"
#include "Encoder.h"
#include <functional>
#include "DriverCommand.pb.h"
Driver::Driver() :
	_io(new IoService()),
	m_running(false),
	m_hardware(_io),
	m_messenger(_io->GetIOService()),
	m_statusPush(_io->GetIOService(), boost::posix_time::millisec(250)),
	m_hapticsPull(_io->GetIOService(), boost::posix_time::millisec(5)),
	m_commandPull(_io->GetIOService(), boost::posix_time::millisec(50)),
	m_imus()

{

	m_hardware.RegisterPacketCallback(SuitPacket::PacketType::ImuData, [this](auto packet) {
		m_imus.ConsumePacket(packet); 
	});
}

Driver::~Driver()
{
}

bool Driver::StartThread()
{
	m_running = true;

	m_hapticsPull.SetEvent(std::bind(&Driver::handleHaptics, this));
	m_hapticsPull.Start();

	m_statusPush.SetEvent(std::bind(&Driver::handleStatus, this));
	m_statusPush.Start();

	m_commandPull.SetEvent(std::bind(&Driver::handleCommands, this));
	m_commandPull.Start();
	return true;
}

bool Driver::Shutdown()
{
	m_running.store(false);

	m_statusPush.Stop();
	m_hapticsPull.Stop();
	m_commandPull.Stop();

	m_messenger.Disconnect();
	
	_io->Shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return true;
}

void Driver::handleHaptics()
{
	if (auto commands = m_messenger.ReadHaptics()) {
		for (const auto& command : *commands) {
			m_hardware.ReceiveExecutionCommand(command);
		}
	}
}

void Driver::handleStatus()
{
	m_messenger.WriteSuits(m_hardware.PollDevice());
}

void Driver::handleCommands()
{
	if (auto commands = m_messenger.ReadCommands()) {
		for (const auto& command : *commands) {
			switch (command.command()) {
			case NullSpaceIPC::DriverCommand_Command_DISABLE_TRACKING:
				m_hardware.DisableTracking();
				break;
			case NullSpaceIPC::DriverCommand_Command_ENABLE_TRACKING:
				m_hardware.EnableTracking();
				break;
			default: 
				break;
			}
		}
	}
}


