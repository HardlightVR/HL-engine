#include "stdafx.h"
#include "FirmwareInterface.h"
#include "Locator.h"
#include "InstructionSet.h"
#include "HardwareCommandVisitor.h"
#include "logger.h"
#include "Instructions.h"
FirmwareInterface::FirmwareInterface(const std::string& data_dir, BoostSerialAdapter* adapter, boost::asio::io_service& io):
m_instructionSet(std::make_shared<InstructionSet>(data_dir)),
_adapter(adapter),
_builder(m_instructionSet),
_writeTimer(io),
_batchingDeadline(io),
_writeInterval(20),
_batchingTimeout(20),
BATCH_SIZE(192),
_lfQueue(10240),
_isBatching(false),
m_totalBytesSent(0)

{
	_adapter->OnPacketVersionChange([this](PacketVersion version) { 
		m_packetVersion = version;  
	});

	_writeTimer.expires_from_now(_writeInterval);
	_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });

}






void FirmwareInterface::writeBuffer() {
	const std::size_t avail = _lfQueue.read_available();
	if (avail == 0) {

		_writeTimer.expires_from_now(_writeInterval);
		_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });
	}
	else if (avail > 0 && avail < BATCH_SIZE) {
		if (_isBatching) {

		
			_writeTimer.expires_from_now(_writeInterval);
			_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });
			return;
		}


		_isBatching = true;
		_batchingDeadline.expires_from_now(_batchingTimeout);
		_batchingDeadline.async_wait([&](const boost::system::error_code& ec) {
			if (!ec) {
				auto a = std::make_shared<uint8_t*>(new uint8_t[BATCH_SIZE]);
				const int actualLen = _lfQueue.pop(*a, BATCH_SIZE);

				this->_adapter->Write(a, actualLen, [this](const boost::system::error_code& e, std::size_t bytes_t) {
					if (e) {
						core_log(nsvr_severity_warning, "FirmwareInterface", "Failed to write to suit while batching");
					}
					else {
						m_totalBytesSent += bytes_t;
					}
				}
				);
				_writeTimer.expires_from_now(_writeInterval);
				_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });
				_isBatching = false;
			}
		});
	}
	else {
		_isBatching = false;
		_batchingDeadline.cancel();
		auto a = std::make_shared<uint8_t*>(new uint8_t[BATCH_SIZE]);
		const int actualLen = _lfQueue.pop(*a, BATCH_SIZE);

		_adapter->Write(a, actualLen, [this](const boost::system::error_code& ec, std::size_t bytes_t) {

			if (ec) {
				core_log(nsvr_severity_warning, "FirmwareInterface", "Failed to write to suit while writing a full batch");

			}
			else {
				m_totalBytesSent += bytes_t;
			}
		}
		);
		_writeTimer.expires_from_now(_writeInterval);
		_writeTimer.async_wait([&](const boost::system::error_code& ec) { writeBuffer(); });
	}

}

void FirmwareInterface::EnableTracking()
{

	VerifyThenExecute(_builder.UseInstruction("IMU_ENABLE"), nsvr::config::Instruction(nsvr::config::InstructionId::IMU_ENABLE, m_packetVersion, {}));
}

void FirmwareInterface::DisableTracking()
{
	VerifyThenExecute(_builder.UseInstruction("IMU_DISABLE"));
}

void FirmwareInterface::RequestSuitVersion()
{
	VerifyThenExecute(_builder.UseInstruction("GET_VERSION"));
}

//Todo: update all to VerifyThenExecute
void FirmwareInterface::ReadDriverData(Location loc, Register reg)
{
	VerifyThenExecute(
		_builder.UseInstruction("READ_DATA")
		.WithParam("zone", Locator::Translator().ToString(loc))
		.WithParam("register", static_cast<int>(reg))
	); 
}

void FirmwareInterface::ResetDrivers()
{
	VerifyThenExecute(_builder.UseInstruction("RESET_DRIVERS"));
}

void FirmwareInterface::VerifyThenExecute(InstructionBuilder& builder) {
	if (builder.Verify()) {
		queue_packet(builder.Build());
	}
	else {
		core_log(nsvr_severity_error, "FirmwareInterface", std::string("Failed to build instruction: " + builder.GetDebugString()));
	}
}

void FirmwareInterface::VerifyThenExecute(InstructionBuilder& builder, const nsvr::config::Instruction& alternateInst) {
	//bool normalVerify = builder.Verify();
	bool alternateVerify = nsvr::config::Verify(alternateInst, m_instructionSet.get());

	//assert(normalVerify == alternateVerify);

//	auto normalPacket = builder.Build();
	auto alternatePacket = nsvr::config::Build(alternateInst);

//	assert(normalPacket == alternatePacket);

	if (alternateVerify) {
		queue_packet(alternatePacket);
	}
	else {
		core_log(nsvr_severity_error, "FirmwareInterface", std::string("Failed to build instruction: " + builder.GetDebugString()));
	}
}
void FirmwareInterface::EnableAudioMode(Location pad, const FirmwareInterface::AudioOptions& opts)
{
	VerifyThenExecute(
		_builder.UseInstruction("AUDIO_MODE_ENABLE")
		.WithParam("zone", Locator::Translator().ToString(pad))
		.WithParam("audio_max", opts.AudioMax) // 0 -255
		.WithParam("audio_min", opts.AudioMin)// 0-255
		.WithParam("peak_time", opts.PeakTime) //0-3
		.WithParam("filter", opts.Filter) //0-3
	);
}

void FirmwareInterface::EnableIntrigMode(Location pad)
{
	VerifyThenExecute(
		_builder.UseInstruction("INTRIG_MODE_ENABLE")
		.WithParam("zone", Locator::Translator().ToString(pad))
	);
}

void FirmwareInterface::EnableRtpMode(Location pad)
{
	VerifyThenExecute(
		_builder.UseInstruction("RTP_MODE_ENABLE")
		.WithParam("zone", Locator::Translator().ToString(pad))
	);
}

void FirmwareInterface::PlayRtp(Location location, int strength)
{
	VerifyThenExecute(
		_builder.UseInstruction("PLAY_RTP")
		.WithParam("zone", Locator::Translator().ToString(location))
		.WithParam("volume", strength)
	);
}

void FirmwareInterface::Ping()
{
	VerifyThenExecute(_builder.UseInstruction("STATUS_PING"), nsvr::config::Instruction(nsvr::config::InstructionId::STATUS_PING, m_packetVersion, {}));

}

void FirmwareInterface::RawCommand(const uint8_t * bytes, std::size_t length)
{
	_lfQueue.push(bytes, length);

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

	
	VerifyThenExecute(_builder.UseInstruction("PLAY_EFFECT")
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
	
	VerifyThenExecute(_builder.UseInstruction("PLAY_CONTINUOUS")
		.WithParam("effect", Locator::Translator().ToString(actualEffect))
		.WithParam("zone", Locator::Translator().ToString(location)));
}

void FirmwareInterface::queue_packet(const std::vector<uint8_t>& packet)
{
	_lfQueue.push(packet.data(), packet.size());

}


void FirmwareInterface::HaltEffect(Location location)
{
	

	VerifyThenExecute(_builder.UseInstruction("HALT_SINGLE")
		.WithParam("zone", Locator::Translator().ToString(location)));
}
