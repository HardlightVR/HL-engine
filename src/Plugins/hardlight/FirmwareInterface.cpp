#include "stdafx.h"
#include "FirmwareInterface.h"
#include "Locator.h"
#include "InstructionSet.h"
#include "HardwareCommandVisitor.h"
#include "logger.h"
#include "Instructions.h"

constexpr unsigned int BATCH_SIZE = 192;

FirmwareInterface::FirmwareInterface(const std::string& data_dir, BoostSerialAdapter* adapter, boost::asio::io_service& io)
	: m_queue(10240)
	, m_instructionSet(std::make_shared<InstructionSet>(data_dir))
	, m_instructionBuilder(m_instructionSet)
	, m_serial(adapter)
	, m_packetVersion()
	, m_isBatching(false)
	, m_totalBytesSent(0)
	, m_writeInterval(20)
	, m_writeTimer(io)
	, m_batchingTimeout(20)
	, m_batchingDeadline(io)

{
	m_serial->OnPacketVersionChange([this](PacketVersion version) { 
		m_packetVersion = version;  
	});

	m_writeTimer.expires_from_now(m_writeInterval);
	m_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });

}






void FirmwareInterface::writeBuffer() {
	const std::size_t avail = m_queue.read_available();
	if (avail == 0) {

		m_writeTimer.expires_from_now(m_writeInterval);
		m_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });
	}
	else if (avail > 0 && avail < BATCH_SIZE) {
		if (m_isBatching) {

		
			m_writeTimer.expires_from_now(m_writeInterval);
			m_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });
			return;
		}


		m_isBatching = true;
		m_batchingDeadline.expires_from_now(m_batchingTimeout);
		m_batchingDeadline.async_wait([&](const boost::system::error_code& ec) {
			if (!ec) {
				auto a = std::make_shared<uint8_t*>(new uint8_t[BATCH_SIZE]);
				const int actualLen = m_queue.pop(*a, BATCH_SIZE);

				this->m_serial->Write(a, actualLen, [this](const boost::system::error_code& e, std::size_t bytes_t) {
					if (e) {
						core_log(nsvr_severity_warning, "FirmwareInterface", "Failed to write to suit while batching");
					}
					else {
						m_totalBytesSent += bytes_t;
					}
				}
				);
				m_writeTimer.expires_from_now(m_writeInterval);
				m_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });
				m_isBatching = false;
			}
		});
	}
	else {
		m_isBatching = false;
		m_batchingDeadline.cancel();
		auto a = std::make_shared<uint8_t*>(new uint8_t[BATCH_SIZE]);
		const int actualLen = m_queue.pop(*a, BATCH_SIZE);
		
		m_serial->Write(a, actualLen, [this](const boost::system::error_code& ec, std::size_t bytes_t) {

			if (ec) {
				core_log(nsvr_severity_warning, "FirmwareInterface", "Failed to write to suit while writing a full batch");

			}
			else {
				m_totalBytesSent += bytes_t;
			}
		}
		);
		m_writeTimer.expires_from_now(m_writeInterval);
		m_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });
	}

}

void FirmwareInterface::EnableTracking()
{

	verifyThenQueue(m_instructionBuilder.UseInstruction("IMU_ENABLE"), nsvr::config::Instruction(nsvr::config::InstructionId::IMU_ENABLE, m_packetVersion, {}));
}

void FirmwareInterface::DisableTracking()
{
	verifyThenQueue(m_instructionBuilder.UseInstruction("IMU_DISABLE"));
}

void FirmwareInterface::RequestSuitVersion()
{
	verifyThenQueue(m_instructionBuilder.UseInstruction("GET_VERSION"));
}

//Todo: update all to verifyThenQueue
void FirmwareInterface::ReadDriverData(Location loc, Register reg)
{
	verifyThenQueue(
		m_instructionBuilder.UseInstruction("READ_DATA")
		.WithParam("zone", Locator::Translator().ToString(loc))
		.WithParam("register", static_cast<int>(reg))
	); 
}

void FirmwareInterface::ResetDrivers()
{
	verifyThenQueue(m_instructionBuilder.UseInstruction("RESET_DRIVERS"));
}

void FirmwareInterface::verifyThenQueue(InstructionBuilder& builder) {
	if (builder.Verify()) {
		queuePacket(builder.Build());
	}
	else {
		core_log(nsvr_severity_error, "FirmwareInterface", std::string("Failed to build instruction: " + builder.GetDebugString()));
	}
}

void FirmwareInterface::verifyThenQueue(InstructionBuilder& builder, const nsvr::config::Instruction& alternateInst) {
	//bool normalVerify = builder.Verify();
	bool alternateVerify = nsvr::config::Verify(alternateInst, m_instructionSet.get());

	//assert(normalVerify == alternateVerify);

//	auto normalPacket = builder.Build();
	auto alternatePacket = nsvr::config::Build(alternateInst);

//	assert(normalPacket == alternatePacket);

	if (alternateVerify) {
		queuePacket(alternatePacket);
	}
	else {
		core_log(nsvr_severity_error, "FirmwareInterface", std::string("Failed to build instruction: " + builder.GetDebugString()));
	}
}
void FirmwareInterface::EnableAudioMode(Location pad, const FirmwareInterface::AudioOptions& opts)
{
	verifyThenQueue(
		m_instructionBuilder.UseInstruction("AUDIO_MODE_ENABLE")
		.WithParam("zone", Locator::Translator().ToString(pad))
		.WithParam("audio_max", opts.AudioMax) // 0 -255
		.WithParam("audio_min", opts.AudioMin)// 0-255
		.WithParam("peak_time", opts.PeakTime) //0-3
		.WithParam("filter", opts.Filter) //0-3
	);
}

void FirmwareInterface::EnableIntrigMode(Location pad)
{
	verifyThenQueue(
		m_instructionBuilder.UseInstruction("INTRIG_MODE_ENABLE")
		.WithParam("zone", Locator::Translator().ToString(pad))
	);
}

void FirmwareInterface::EnableRtpMode(Location pad)
{
	verifyThenQueue(
		m_instructionBuilder.UseInstruction("RTP_MODE_ENABLE")
		.WithParam("zone", Locator::Translator().ToString(pad))
	);
}

void FirmwareInterface::PlayRtp(Location location, int strength)
{
	verifyThenQueue(
		m_instructionBuilder.UseInstruction("PLAY_RTP")
		.WithParam("zone", Locator::Translator().ToString(location))
		.WithParam("volume", strength)
	);
}

void FirmwareInterface::Ping()
{
	verifyThenQueue(m_instructionBuilder.UseInstruction("STATUS_PING"), nsvr::config::Instruction(nsvr::config::InstructionId::STATUS_PING, m_packetVersion, {}));

}

void FirmwareInterface::RawCommand(const uint8_t * bytes, std::size_t length)
{
	m_queue.push(bytes, length);

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

	
	verifyThenQueue(m_instructionBuilder.UseInstruction("PLAY_EFFECT")
		.WithParam("zone", Locator::Translator().ToString(location))
		.WithParam("effect", Locator::Translator().ToString(actualEffect)));

}

void FirmwareInterface::PlayEffectContinuous(Location location, uint32_t effect, float strength)
{



	std::string effectString = Locator::Translator().ToString(effect);
	if (m_instructionSet->Atoms().find(effectString) == m_instructionSet->Atoms().end()) {
		core_log(nsvr_severity_error, "FirmwareInterface", std::string("Failed to find atom'" + effectString + "' in the instruction set"));
		return;
	}

	auto actualEffect = m_instructionSet->Atoms().at(effectString).GetEffect(strength);
	
	verifyThenQueue(m_instructionBuilder.UseInstruction("PLAY_CONTINUOUS")
		.WithParam("effect", Locator::Translator().ToString(actualEffect))
		.WithParam("zone", Locator::Translator().ToString(location)));
}

void FirmwareInterface::queuePacket(const std::vector<uint8_t>& packet)
{
	m_queue.push(packet.data(), packet.size());
}


void FirmwareInterface::HaltEffect(Location location)
{
	

	verifyThenQueue(m_instructionBuilder.UseInstruction("HALT_SINGLE")
		.WithParam("zone", Locator::Translator().ToString(location)));
}
