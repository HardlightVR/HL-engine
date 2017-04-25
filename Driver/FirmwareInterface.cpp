#include "stdafx.h"
#include "FirmwareInterface.h"
#include "Locator.h"
#include "InstructionSet.h"

#include <boost\log\trivial.hpp>
FirmwareInterface::FirmwareInterface(std::unique_ptr<ICommunicationAdapter>& adapter, boost::asio::io_service& io):
m_instructionSet(std::make_shared<InstructionSet>()),
_adapter(adapter),
_builder(m_instructionSet),
_writeTimer(io),
_batchingDeadline(io),
_writeInterval(20),
_batchingTimeout(20),
BATCH_SIZE(64),
_lfQueue(1024)

{

	_writeTimer.expires_from_now(_writeInterval);
	_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
}


FirmwareInterface::~FirmwareInterface()
{
}

void FirmwareInterface::writeBuffer() {
	const std::size_t avail = _lfQueue.read_available();
	if (avail == 0) {
		_writeTimer.expires_from_now(_writeInterval);
		_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
	}
	else if (avail > 0 && avail < BATCH_SIZE) {
		if (_isBatching) {
			//	std::cout << "psst! I'm waiting for a batch of cookies!" << '\n';
			_writeTimer.expires_from_now(_writeInterval);
			_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
			return;
		}

		//BOOST_LOG_TRIVIAL(trace) << "[FI] Need to make new packet batch";

		//	std::cout << "Okay, we need to cook a new batch\n";
		_isBatching = true;
		_batchingDeadline.expires_from_now(_batchingTimeout);
		_batchingDeadline.async_wait([&](const boost::system::error_code& ec) {
			if (!ec) {
				auto a = std::make_shared<uint8_t*>(new uint8_t[BATCH_SIZE]);
				const int actualLen = _lfQueue.pop(*a, BATCH_SIZE);
				//	std::cout << "had to send a mini batch of " << actualLen << " cookies\n";
				//BOOST_LOG_TRIVIAL(trace) << "[FI] Sent an undersized packet batch";

				this->_adapter->Write(a, actualLen, [&](const boost::system::error_code& e, std::size_t bytes_t) {
					if (e) {
						BOOST_LOG_TRIVIAL(info) << "[FI] Failed to write an undersized packet batch: " << e.message();
					}
				}
				);
				_writeTimer.expires_from_now(_writeInterval);
				_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
				_isBatching = false;
			}
		});
		//	std::cout << "Some avail, waiting" << '\n';
	}
	else {
		_isBatching = false;
		_batchingDeadline.cancel();
		auto a = std::make_shared<uint8_t*>(new uint8_t[BATCH_SIZE]);
		const int actualLen = _lfQueue.pop(*a, BATCH_SIZE);
		_adapter->Write(a, actualLen, [&](const boost::system::error_code& ec, std::size_t bytes_t) {

			if (ec) {
				BOOST_LOG_TRIVIAL(info) << "[FI] Failed to write a full packet batch";

			}
		}
		);
		_writeTimer.expires_from_now(_writeInterval);
		_writeTimer.async_wait(boost::bind(&FirmwareInterface::writeBuffer, this));
	}

}

void FirmwareInterface::EnableTracking()
{
	if (_builder.UseInstruction("IMU_ENABLE").Verify()) {
		chooseExecutionStrategy(_builder.Build());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to build instruction " << _builder.GetDebugString();
	}
}

void FirmwareInterface::DisableTracking()
{
	if (_builder.UseInstruction("IMU_DISABLE").Verify()) {
		chooseExecutionStrategy(_builder.Build());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to build instruction " << _builder.GetDebugString();
	}
}

void FirmwareInterface::RequestSuitVersion()
{
	if (_builder.UseInstruction("GET_VERSION").Verify()) {
		chooseExecutionStrategy(_builder.Build());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to build instruction " << _builder.GetDebugString();
	}
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
	if (_builder.UseInstruction("RESET_DRIVERS").Verify()) {
		chooseExecutionStrategy(_builder.Build());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to build instruction " << _builder.GetDebugString();
	}
}

void FirmwareInterface::VerifyThenExecute(InstructionBuilder& builder) {
	if (builder.Verify()) {
		chooseExecutionStrategy(builder.Build());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to build instruction " << builder.GetDebugString();

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

void FirmwareInterface::RawCommand(const uint8_t * bytes, std::size_t length)
{
	_lfQueue.push(bytes, length);

}

void FirmwareInterface::PlayEffect(Location location, std::string effectString, float strength) {
	
	if (m_instructionSet->Atoms().find(effectString) == m_instructionSet->Atoms().end()) {
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to find atom '" << effectString << "' in the instruction set";
		return;
	}

	auto effect  = m_instructionSet->Atoms().at(effectString).GetEffect(strength);
	if (_builder.UseInstruction("PLAY_EFFECT")
		.WithParam("zone", Locator::Translator().ToString(location))
		.WithParam("effect", Locator::Translator().ToString(effect))
		.Verify())
	{
		chooseExecutionStrategy(_builder.Build());

	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to build instruction " << _builder.GetDebugString();
	}
}

void FirmwareInterface::PlayEffectContinuous(Location location, std::string effectString, float strength)
{
	if (m_instructionSet->Atoms().find(effectString) == m_instructionSet->Atoms().end()) {
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to find atom '" << effectString << "' in the instruction set";
		return;
	}

	auto effect = m_instructionSet->Atoms().at(effectString).GetEffect(strength);
	if (_builder.UseInstruction("PLAY_CONTINUOUS")
		.WithParam("effect", Locator::Translator().ToString(effect))
		.WithParam("zone", Locator::Translator().ToString(location))
		.Verify())
	{
		chooseExecutionStrategy(_builder.Build());
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to build instruction " << _builder.GetDebugString();
	}
}

void FirmwareInterface::chooseExecutionStrategy(const Packet & packet)
{
	_lfQueue.push(packet.Data, packet.Data + packet.Length);

}


void FirmwareInterface::HaltEffect(Location location)
{
	if (_builder.UseInstruction("HALT_SINGLE")
		.WithParam("zone", Locator::Translator().ToString(location))
		.Verify())
	{
		chooseExecutionStrategy(_builder.Build());
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "[FI] Failed to build instruction " << _builder.GetDebugString();
	}

}
