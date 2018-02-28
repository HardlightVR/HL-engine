#include "stdafx.h"
#include "FirmwareInterface.h"
#include "Locator.h"
#include "InstructionSet.h"
#include "HardwareCommandVisitor.h"
#include "logger.h"
#include "Instructions.h"
#include "IMU_ID.h"
constexpr unsigned int BATCH_SIZE = 16;

FirmwareInterface::FirmwareInterface(const std::string& data_dir, HardwareIO& outgoing, boost::asio::io_service& io)
	: m_instructionSet(std::make_shared<InstructionSet>(data_dir))
	, m_outgoing(outgoing)
	, m_packetVersion(PacketVersion::MarkIII)
	, m_isBatching(false)
	, m_totalBytesSent(0)
	, m_packetLock()
	, m_imuIds{ 0x3c, 0x3a, 0x39 }

{

	m_instructionSet->LoadAll();

}







void FirmwareInterface::RequestTrackingStatus()
{
	for (auto sensor : m_imuIds) {
		queueInstruction(inst::get_track_status(inst::sensor(sensor)));
	}
}

void FirmwareInterface::EnableTracking()
{
	queueInstruction(inst::set_track_enable());
}

void FirmwareInterface::DisableTracking()
{
	queueInstruction(inst::set_track_disable());
}

void FirmwareInterface::RequestSuitVersion()
{
	queueInstruction(inst::get_version());
}

void FirmwareInterface::GetMotorStatus(Location location)
{
	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(location));

	queueInstruction(inst::get_motor_status(inst::motor(actualZone)));
}






void FirmwareInterface::EnableAudioMode(Location pad, const AudioOptions& opts)
{
	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(pad));
	
	queueInstruction(inst::set_motor_audiomode(
		inst::motor(actualZone),
		inst::audio_vibe(opts.VibeCtrl),
		inst::audio_minlevel(opts.AudioMin),
		inst::audio_maxlevel(opts.AudioMax),
		inst::audio_mindrv(opts.MinDrv),
		inst::audio_maxdrv(opts.MaxDrv)
	));
}

void FirmwareInterface::DisableAudioMode(Location pad)
{
	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(pad));


	//queueInstruction(inst::set_motor_stop_audio(inst::motor(actualZone)));
	// ^^^ doesn't work

	queueInstruction(inst::set_motor_intrigmode(inst::motor(actualZone)));
	queueInstruction(inst::set_motor_play_effect(inst::motor(actualZone), inst::effect(1)));
}

void FirmwareInterface::EnableIntrigMode(Location pad)
{
	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(pad));
	queueInstruction(inst::set_motor_intrigmode(inst::motor(actualZone)));
}

void FirmwareInterface::EnableRtpMode(Location pad)
{

	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(pad));
	queueInstruction(inst::set_motor_rtpmode(inst::motor(actualZone)));
}

void FirmwareInterface::PlayRtp(Location location, int strength)
{

	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(location));
	assert(strength >= 0 && strength <= 255);
	queueInstruction(inst::set_motor_play_rtp(inst::motor(actualZone), inst::volume(strength)));
}

void FirmwareInterface::Ping()
{
	queueInstruction(inst::get_ping());
}

void FirmwareInterface::RequestTrackingData()
{
	for (auto imu : m_imuIds) {
		queueInstruction(inst::get_track_data(inst::sensor(imu)));
	}
}

void FirmwareInterface::RequestTrackingCompass()
{
	for (auto imu : m_imuIds) {
		queueInstruction(inst::get_track_compass(inst::sensor(imu)));
	}
}

void FirmwareInterface::RequestTrackingGravity()
{
	for (auto imu : m_imuIds) {
		queueInstruction(inst::get_track_gravity(inst::sensor(imu)));
	}
}





std::size_t FirmwareInterface::GetTotalBytesSent() const
{
	return m_totalBytesSent;
}

const InstructionSet * FirmwareInterface::GetInstructions() const
{
	return m_instructionSet.get();
}

void FirmwareInterface::Execute(const CommandBuffer & buffer)
{
	HardwareCommandVisitor executor(*this);
	for (const auto& command : buffer) {
		boost::apply_visitor(executor, command);
	}
}

void FirmwareInterface::PlayEffect(Location location, uint32_t effect, float strength) {


	std::string effectString = Locator::Translator().ToString(effect);
	if (m_instructionSet->Atoms().find(effectString) == m_instructionSet->Atoms().end()) {
		core_log(nsvr_severity_error, "FirmwareInterface", std::string("Failed to find atom'" + effectString + "' in the instruction set"));
		return;
	}

	auto actualEffect  = m_instructionSet->Atoms().at(effectString).GetEffect(strength);
	uint8_t actualId  = m_instructionSet->ParamDict().at("effect").at(Locator::Translator().ToString(actualEffect));
	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(location));

	queueInstruction(inst::set_motor_play_effect(inst::motor(actualZone), inst::effect(actualId)));

}



void FirmwareInterface::PlayEffectContinuous(Location location, uint32_t effect, float strength)
{



	std::string effectString = Locator::Translator().ToString(effect);
	if (m_instructionSet->Atoms().find(effectString) == m_instructionSet->Atoms().end()) {
		core_log(nsvr_severity_error, "FirmwareInterface", std::string("Failed to find atom'" + effectString + "' in the instruction set"));
		return;
	}

	auto actualEffect = m_instructionSet->Atoms().at(effectString).GetEffect(strength);
	uint8_t actualId = m_instructionSet->ParamDict().at("effect").at(Locator::Translator().ToString(actualEffect));
	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(location));
	
	queueInstruction(inst::set_motor_play_continuous(inst::motor(actualZone), inst::effect(actualId)));
}

void FirmwareInterface::RequestUuid()
{
	queueInstruction(inst::get_uuid());
}



void FirmwareInterface::HaltEffect(Location location)
{

	auto actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(location));

	queueInstruction(inst::set_motor_halt_single(inst::motor(actualZone)));

}
