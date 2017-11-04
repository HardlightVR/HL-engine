#include "stdafx.h"
#include "FirmwareInterface.h"
#include "Locator.h"
#include "InstructionSet.h"
#include "HardwareCommandVisitor.h"
#include "logger.h"
#include "Instructions.h"
#include "IMU_ID.h"
constexpr unsigned int BATCH_SIZE = 16;

FirmwareInterface::FirmwareInterface(const std::string& data_dir, std::shared_ptr<boost::lockfree::spsc_queue<uint8_t>> outgoing, boost::asio::io_service& io)
	: m_instructionSet(std::make_shared<InstructionSet>(data_dir))
	, m_instructionBuilder(m_instructionSet)
	, m_outgoing(outgoing)
	, m_packetVersion(PacketVersion::MarkIII)
	, m_isBatching(false)
	, m_totalBytesSent(0)

{
	/*m_serial->OnPacketVersionChange([this](PacketVersion version) { 
		m_packetVersion = version;  
	});*/

	

}

FirmwareInterface::~FirmwareInterface()
{
	//m_writeTimer.cancel();
//	m_batchingDeadline.cancel();
}






void FirmwareInterface::RequestTrackingStatus()
{
	auto sensors = { 0x3c, 0x3a };
	for (auto sensor : sensors) {
		verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::GET_TRACK_STATUS, m_packetVersion, { {"sensor", sensor } }));
	}

}

void FirmwareInterface::EnableTracking()
{

	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::SET_TRACK_ENABLE, m_packetVersion, {}));
}

void FirmwareInterface::DisableTracking()
{
	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::SET_TRACK_DISABLE, m_packetVersion, {}));
}

void FirmwareInterface::RequestSuitVersion()
{
	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::GET_VERSION, PacketVersion::MarkIII, {}));
}




void FirmwareInterface::verifyThenQueue(InstructionBuilder& builder) {
	if (builder.Verify()) {
		queuePacket(builder.Build());
	}
	else {
		core_log(nsvr_severity_error, "FirmwareInterface", std::string("Failed to build instruction: " + builder.GetDebugString()));
	}
}

void FirmwareInterface::verifyThenQueue(const nsvr::config::Instruction& inst) {
	bool alternateVerify = nsvr::config::Verify(inst, m_instructionSet.get());
	if (alternateVerify) {
		queuePacket(nsvr::config::Build(inst));
	}
	else {
		core_log(nsvr_severity_error, "FirmwareInterface", std::string("Failed to build instruction: " + nsvr::config::HumanReadable(inst)));
	}
}

void FirmwareInterface::EnableAudioMode(Location pad, const FirmwareInterface::AudioOptions& opts)
{
	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(pad));

	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::SET_MOTOR_PLAY_AUDIO, m_packetVersion, {
		{"zone", actualZone},
		{"vibectrl", opts.VibeCtrl},
		{ "minlevel", opts.AudioMin },
		{ "maxlevel", opts.AudioMax },
		{ "mindrv", opts.MinDrv },
		{"maxdrv", opts.MaxDrv}
	}));
}

void FirmwareInterface::EnableIntrigMode(Location pad)
{
	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(pad));
	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::SET_MOTOR_INTRIGMODE, m_packetVersion, { { "zone", actualZone } }));
}

void FirmwareInterface::EnableRtpMode(Location pad)
{

	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(pad));
	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::SET_MOTOR_RTPMODE, m_packetVersion, { {"zone", actualZone} }));
	
}

void FirmwareInterface::PlayRtp(Location location, int strength)
{

	uint8_t actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(location));
	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::SET_MOTOR_PLAY_RTP, m_packetVersion, { { "zone", actualZone }, {"volume", strength} }));
	
}

void FirmwareInterface::Ping()
{
	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::GET_PING, m_packetVersion, {}));

}




std::size_t FirmwareInterface::GetTotalBytesSent() const
{
	return m_totalBytesSent;
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
	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::SET_MOTOR_PLAY_EFFECT, m_packetVersion, { { "zone", actualZone } , {"effect", actualId} }));

	
	/*verifyThenQueue(m_instructionBuilder.UseInstruction("PLAY_EFFECT")
		.WithParam("zone", Locator::Translator().ToString(location))
		.WithParam("effect", Locator::Translator().ToString(actualEffect)));*/

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
	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::SET_MOTOR_PLAY_CONTINUOUS, m_packetVersion, { { "zone", actualZone } ,{ "effect", actualId } }));
}

void FirmwareInterface::RequestUuid()
{
	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::GET_UUID, m_packetVersion, {}));
}

void FirmwareInterface::queuePacket(const std::vector<uint8_t>& packet)
{
	m_outgoing->push(packet.data(), packet.size());
}


void FirmwareInterface::HaltEffect(Location location)
{

	auto actualZone = m_instructionSet->ParamDict().at("zone").at(Locator::Translator().ToString(location));

	verifyThenQueue(nsvr::config::Instruction(nsvr::config::InstructionId::SET_MOTOR_HALT_SINGLE, m_packetVersion, { { "zone", actualZone} }));

}
