#include "stdafx.h"
#include "Driver.h"
#include "IoService.h"
#include "ClientMessenger.h" 
#include "IntermediateHapticFormats.h"
#include "Encoder.h"
#include <functional>
#include "DriverCommand.pb.h"
#include "SuitVersionInfo.h"
Driver::Driver() :
	_io(new IoService()),
	m_hardware(_io),
	m_messenger(_io->GetIOService()),
	m_statusPush(_io->GetIOService(), boost::posix_time::millisec(250)),
	m_hapticsPull(_io->GetIOService(), boost::posix_time::millisec(5)),
	m_commandPull(_io->GetIOService(), boost::posix_time::millisec(50)),
	m_trackingPush(_io->GetIOService(), boost::posix_time::millisec(10)),
	m_imus(),
	m_cachedTracking({})

{

	m_hardware.RegisterPacketCallback(SuitPacket::PacketType::ImuData, [this](auto packet) {
		m_imus.ConsumePacket(packet); 
	});

	m_hardware.RegisterPacketCallback(SuitPacket::PacketType::SuitVersion, [this](auto packet) {
		SuitVersionInfo version(packet);
		if (version.Major == 2 && version.Minor == 5) {
			m_imus.AssignMapping(1, Imu::Left_Upper_Arm);
			m_imus.AssignMapping(3, Imu::Right_Upper_Arm);
			m_imus.AssignMapping(0, Imu::Chest);
		}
		else if (version.Major == 2 && version.Minor == 3) {
			m_imus.AssignMapping(0, Imu::Chest);
		}
		else if (version.Major == 2 && version.Minor == 4) {
			m_imus.AssignMapping(3, Imu::Chest);
		}
	});
}

Driver::~Driver()
{
}

bool Driver::StartThread()
{

	m_hapticsPull.SetEvent([this]() { handleHaptics(); });
	m_hapticsPull.Start();

	m_statusPush.SetEvent([this]() { handleStatus(); });
	m_statusPush.Start();

	m_commandPull.SetEvent([this]() {handleCommands(); });
	m_commandPull.Start();

	m_trackingPush.SetEvent([this]() {handleTracking(); });
	m_trackingPush.Start();
	return true;
}

bool Driver::Shutdown()
{

	m_statusPush.Stop();
	m_hapticsPull.Stop();
	m_commandPull.Stop();
	m_trackingPush.Stop();
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

void Driver::handleTracking()
{
	NullSpace::SharedMemory::TrackingUpdate update = m_cachedTracking;

	if (auto quat = m_imus.GetOrientation(Imu::Chest)) {
		update.chest = *quat;
	}

	if (auto quat = m_imus.GetOrientation(Imu::Left_Upper_Arm)) {
		update.left_upper_arm = *quat;
	}

	if (auto quat = m_imus.GetOrientation(Imu::Right_Upper_Arm)) {
		update.right_upper_arm = *quat;
	}
	
	m_cachedTracking = update;

	m_messenger.WriteTracking(update);
}

